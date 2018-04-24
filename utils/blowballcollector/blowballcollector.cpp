
#include <glog/logging.h>
#include <set>

#include <iota/utils/blowballcollector.hpp>
#include <iota/utils/common/zmqpub.hpp>

using namespace iota::utils;
bool BlowballCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (!conf[SNAPSHOT_INTERVAL] || !conf[PUBLISHER]) {
    return false;
  }

  _zmqPublisher = conf[PUBLISHER].as<std::string>();
  _snapshotInterval = conf[SNAPSHOT_INTERVAL].as<uint32_t>();
  return true;
}

void BlowballCollector::collect() {
  using namespace prometheus;
  VLOG(3) << __FUNCTION__;

  _zmqObservable =
      rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
          [&](auto s) { zmqPublisher(std::move(s), _zmqPublisher); });

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  exposer.RegisterCollectable(registry);

  _families = buildHistogramsMap(registry, {});

  analyzeBlowballsPeriodically();
  refCountPublishedTransactions();
}

void BlowballCollector::analyzeBlowballsPeriodically() {
  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();

  std::vector<double> buckets(40);
  double currInterval = 0;
  std::generate(buckets.begin(), buckets.end(),
                [&currInterval]() { return ++currInterval; });

  auto& thisRef = *this;
  if (_snapshotInterval > 0) {
    pubWorker.schedule_periodically(
        pubThread.now(), std::chrono::seconds(_snapshotInterval),
        [&thisRef, buckets](auto scbl) { thisRef.analyzeBlowballs(buckets); });
  }
}

void BlowballCollector::analyzeBlowballs(const std::vector<double>& buckets) {
  {
    auto refCountIt = _txToRefCount.lock_table();
    auto now = std::chrono::system_clock::now();

    for (const auto& it : refCountIt) {
      _families.at(TX_NUM_APPROVERS).get().Add({}, buckets).Observe(it.second);
      std::chrono::system_clock::time_point lastUpdateTime;
      _txToLastUpdateTime.find(it.first, lastUpdateTime);
      auto timeSinceLastUpdate =
          std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime)
              .count();
      if (timeSinceLastUpdate > EXPIARY_PERIOD) {
        _txToRefCount.erase(it.first);
        _txToLastUpdateTime.erase(it.first);
      }
    }
  }
}

void BlowballCollector::refCountPublishedTransactions() {
  auto counterFn = [](uint8_t& num) { ++num; };
  _zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .subscribe(
          [&](std::shared_ptr<iri::IRIMessage> msg) {

            // assuming no tx is a milestone
            if (msg->type() == iri::IRIMessageType::LMHS) {
              auto lmhs =
                  std::static_pointer_cast<iri::LMHSMessage>(std::move(msg));
              if (_milestones.size() > MAX_NUM_MILESTONES) {
                _milestones.pop_front();
              }
              _milestones.push_back(lmhs->latestSolidMilestoneHash());
            }
            if (msg->type() != iri::IRIMessageType::TX) return;

            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));
            auto now = std::chrono::system_clock::now();

            if (std::find(_milestones.begin(), _milestones.end(),
                          tx->trunk()) == _milestones.end()) {
              _txToRefCount.upsert(tx->trunk(), counterFn, 1);
              _txToLastUpdateTime.insert(tx->trunk(), now);
            }

            if (std::find(_milestones.begin(), _milestones.end(),
                          tx->branch()) == _milestones.end()) {
              _txToRefCount.upsert(tx->branch(), counterFn, 1);
              _txToLastUpdateTime.insert(tx->branch(), std::move(now));
            }

          },
          []() {});
}

using namespace prometheus;

PrometheusCollector::HistogramsMap BlowballCollector::buildHistogramsMap(
    std::shared_ptr<Registry> registry,
    const std::map<std::string, std::string>& labels) {
  static std::map<std::string, std::string> nameToDesc = {
      {TX_NUM_APPROVERS,
       "Number of transactions approving a single transaction as observed "
       "across multiple nodes"}};

  std::map<std::string, std::reference_wrapper<Family<Histogram>>> families;
  for (const auto& kv : nameToDesc) {
    auto& curr_family = BuildHistogram()
                            .Name("topology_mapper_" + kv.first)
                            .Help(kv.second)
                            .Labels(labels)
                            .Register(*registry);

    families.insert(
        std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return std::move(families);
}