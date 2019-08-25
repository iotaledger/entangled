
#include <glog/logging.h>
#include <set>

#include "tanglescope/blowballcollector.hpp"
#include "tanglescope/common/zmqpub.hpp"

using namespace iota::tanglescope;

std::map<std::string, std::string> BlowballCollector::nameToDescHistogram = {
    {BlowballCollector::TX_NUM_APPROVERS, "Number of transactions directly approving a single transaction"}};

bool BlowballCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (!conf[SNAPSHOT_INTERVAL] || !conf[PUBLISHER] || !conf[HISTOGRAM_RANGE] || !conf[BUCKET_SIZE]) {
    return false;
  }

  _zmqPublisher = conf[PUBLISHER].as<std::string>();
  _snapshotInterval = conf[SNAPSHOT_INTERVAL].as<uint32_t>();
  _histogramRange = conf[HISTOGRAM_RANGE].as<uint32_t>();
  _bucketSize = conf[BUCKET_SIZE].as<uint32_t>();
  return true;
}

void BlowballCollector::collect() {
  using namespace prometheus;
  VLOG(3) << __FUNCTION__;

  if (urlToZmqObservables.find(_zmqPublisher) == urlToZmqObservables.end()) {
    urlToZmqObservables[_zmqPublisher] = rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
        [&](auto s) { zmqPublisher(std::move(s), _zmqPublisher); });
  }

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  exposer.RegisterCollectable(registry);

  histograms = buildHistogramsMap(registry, "blowball", {}, nameToDescHistogram);

  analyzeBlowballsPeriodically();
  refCountPublishedTransactions();
}

void BlowballCollector::analyzeBlowballsPeriodically() {
  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();

  std::vector<double> buckets(_histogramRange / _bucketSize);
  double currInterval = 0;
  std::generate(buckets.begin(), buckets.end(), [&currInterval, bucketSize = _bucketSize]() {
    currInterval += bucketSize;
    return currInterval;
  });

  auto& thisRef = *this;
  if (_snapshotInterval > 0) {
    pubWorker.schedule_periodically(pubThread.now(), std::chrono::seconds(_snapshotInterval),
                                    [&thisRef, buckets](auto) { thisRef.analyzeBlowballs(buckets); });
  }
}

void BlowballCollector::analyzeBlowballs(const std::vector<double>& buckets) {
  std::set<std::string> expiredRefCounts;
  {
    auto refCountIt = _txToRefCount.lock_table();
    auto now = std::chrono::system_clock::now();

    for (const auto& it : refCountIt) {
      histograms.at(TX_NUM_APPROVERS).get().Add({}, buckets).Observe(it.second);
      std::chrono::system_clock::time_point lastUpdateTime;
      _txToLastUpdateTime.find(it.first, lastUpdateTime);
      auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime).count();
      if (timeSinceLastUpdate > EXPIARY_PERIOD) {
        expiredRefCounts.insert(it.first);
      }
    }
  }
  for (const auto& txHash : expiredRefCounts) {
    _txToRefCount.erase(txHash);
    _txToLastUpdateTime.erase(txHash);
  }
}

void BlowballCollector::refCountPublishedTransactions() {
  auto counterFn = [](uint8_t& num) { ++num; };
  urlToZmqObservables[_zmqPublisher]
      .observe_on(rxcpp::synchronize_new_thread())
      .subscribe(
          [&](std::shared_ptr<iri::IRIMessage> msg) {
            // assuming no tx is a milestone
            if (msg->type() == iri::IRIMessageType::LMHS) {
              auto lmhs = std::static_pointer_cast<iri::LMHSMessage>(std::move(msg));
              if (_milestones.size() > MAX_NUM_MILESTONES) {
                _milestones.pop_front();
              }
              _milestones.push_back(lmhs->latestSolidMilestoneHash());
            }
            if (msg->type() != iri::IRIMessageType::TX) return;

            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));
            auto now = std::chrono::system_clock::now();

            if (std::find(_milestones.begin(), _milestones.end(), tx->trunk()) == _milestones.end()) {
              _txToRefCount.upsert(tx->trunk(), counterFn, 1);
              _txToLastUpdateTime.insert(tx->trunk(), now);
            }

            if (std::find(_milestones.begin(), _milestones.end(), tx->branch()) == _milestones.end()) {
              _txToRefCount.upsert(tx->branch(), counterFn, 1);
              _txToLastUpdateTime.insert(tx->branch(), std::move(now));
            }
          },
          []() {});
}
