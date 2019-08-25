#include <glog/logging.h>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>

#include "tanglescope/tipselectioncollector.hpp"

using namespace iota::tanglescope;
using namespace cppclient;

std::map<std::string, std::string> TipSelectionCollector::nameToDescHistogram = {
    {TipSelectionCollector::NUM_TX_WAS_SELECTED, "# of times tip/tx was selected"},
    {TipSelectionCollector::NUM_TRUNK_EQ_BRANCH, "# of times both selected tips were the same"},
    {TipSelectionCollector::NUM_TX_WAS_NOT_A_TIP, "# of times selected tx was not a tip"},
    {TipSelectionCollector::TIP_SELECTION_DURATION, "The duration_of_tip_selection"}};

bool TipSelectionCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (!conf[IRI_HOST] || !conf[IRI_PORT] || !conf[SAMPLE_INTERVAL] || !conf[SAMPLE_SIZE] || !conf[DEPTH] ||
      !conf[DURATION_HISTOGRAM_RANGE] || !conf[DURATION_BUCKET_SIZE]) {
    return false;
  }

  _iriHost = conf[IRI_HOST].as<std::string>();
  _iriPort = conf[IRI_PORT].as<uint32_t>();
  _sampleInterval = conf[SAMPLE_INTERVAL].as<uint32_t>();
  _sampleSize = conf[SAMPLE_SIZE].as<uint16_t>();
  _depth = conf[DEPTH].as<uint16_t>();
  _durationBucketSize = conf[DURATION_BUCKET_SIZE].as<uint16_t>();
  _durationBucketRange = conf[DURATION_HISTOGRAM_RANGE].as<uint16_t>();
  return true;
}

void TipSelectionCollector::collect() {
  using namespace prometheus;
  VLOG(3) << __FUNCTION__;

  std::vector<double> txSelectionCountBuckets(_sampleSize);
  double currInterval = 0;
  std::generate(txSelectionCountBuckets.begin(), txSelectionCountBuckets.end(),
                [&currInterval]() { return currInterval++; });

  _txSelectionCountBuckets = txSelectionCountBuckets;

  std::vector<double> durationBuckets(_durationBucketRange / _durationBucketSize);
  currInterval = 0;
  std::generate(durationBuckets.begin(), durationBuckets.end(),
                [&currInterval, durationBucketSize = _durationBucketSize]() {
                  currInterval += durationBucketSize;
                  return currInterval;
                });

  _durationBuckets = durationBuckets;

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  exposer.RegisterCollectable(registry);

  _api = std::make_shared<cppclient::BeastIotaAPI>(_iriHost, _iriPort, false);

  _histograms = buildHistogramsMap(registry, "tipselection", {}, nameToDescHistogram);

  queryTipSelectionPeriodically();
}

void TipSelectionCollector::queryTipSelectionPeriodically() {
  auto pubThread = rxcpp::schedulers::make_current_thread();
  auto pubWorker = pubThread.create_worker();

  if (_sampleInterval > 0) {
    pubWorker.schedule_periodically(pubThread.now(), std::chrono::seconds(_sampleInterval),
                                    [&](auto) { queryTipSelection(); });
  } else {
    queryTipSelection();
  }
}

void TipSelectionCollector::queryTipSelection() {
  std::vector<boost::future<nonstd::optional<GetTransactionsToApproveResponse>>> futures;

  uint16_t branchEqTrunkCounter = 0;
  uint16_t numSelectedTXWasNotATip = 0;
  std::unordered_map<std::string, uint16_t> txToNumSelected;
  std::unordered_map<std::string, uint64_t> txToTimeTipSelectionStrated;

  try {
    for (uint16_t i = 0; i < _sampleSize; ++i) {
      auto fu = boost::async(boost::launch::async, [this] { return _api->getTransactionsToApprove(_depth); });

      futures.push_back(std::move(fu));
    }

    for (uint16_t i = 0; i < _sampleSize; ++i) {
      const auto& maybeResp = futures[i].get();
      if (!maybeResp.has_value()) {
        return;
      }

      auto resp = std::move(maybeResp.value());
      ++txToNumSelected[resp.trunkTransaction];
      ++txToNumSelected[resp.branchTransaction];

      auto nowMilliseconds =
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
              .count();

      auto timeTipSelectionRequested = nowMilliseconds - resp.duration;
      txToTimeTipSelectionStrated[resp.trunkTransaction] = timeTipSelectionRequested;
      txToTimeTipSelectionStrated[resp.branchTransaction] = timeTipSelectionRequested;

      _histograms.at(TIP_SELECTION_DURATION).get().Add({}, _durationBuckets).Observe(resp.duration);

      if (resp.trunkTransaction == resp.branchTransaction) {
        ++branchEqTrunkCounter;
      }
    }

    auto& hist = _histograms.at(NUM_TX_WAS_SELECTED).get().Add({}, _txSelectionCountBuckets);

    for (const auto& kv : txToNumSelected) {
      hist.Observe(kv.second);
      std::vector<std::string> txHashes = {kv.first};
      auto approvers = _api->findTransactions({}, {}, {txHashes});

      if (!approvers.empty()) {
        auto transactions = _api->getTransactions(approvers, false);

        for (const auto& approver : transactions) {
          auto txTimestampMS =
              std::chrono::duration_cast<std::chrono::milliseconds>(approver.timestamp.time_since_epoch()).count();
          if (txToTimeTipSelectionStrated[kv.first] > (unsigned long long)txTimestampMS) ++numSelectedTXWasNotATip;
          break;
        }
      }
    }

    _histograms.at(NUM_TRUNK_EQ_BRANCH).get().Add({}, _txSelectionCountBuckets).Observe(branchEqTrunkCounter);

    _histograms.at(NUM_TX_WAS_NOT_A_TIP).get().Add({}, _txSelectionCountBuckets).Observe(numSelectedTXWasNotATip);

  } catch (const std::exception& e) {
    LOG(ERROR) << __FUNCTION__ << " Exception: " << e.what();
  }
}
