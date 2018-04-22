
#include <glog/logging.h>
#include <set>

#include <iota/utils/common/approvedtxdb.hpp>
#include <iota/utils/common/zmqpub.hpp>
#include <iota/utils/topologymapper.hpp>

using namespace iota::utils;
bool TopologyMapper::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (!conf[SNAPSHOT_INTERVAL] || !conf[PUBLISHERS]) {
    return false;
  }

  _zmqPublishers = conf[PUBLISHERS].as<std::list<std::string>>();
  _snapshotInterval = conf[SNAPSHOT_INTERVAL].as<uint32_t>();
  return true;
}

void TopologyMapper::collect() {
  using namespace prometheus;
  VLOG(3) << __FUNCTION__;

  for (const auto& url : _zmqPublishers) {
    auto zmqObservable =
        rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
            [&](auto s) { zmqPublisher(std::move(s), url); });
    _urlToZmqObservables.insert(std::pair(url, zmqObservable));
    _urlToDB[url] = {};
  }

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  exposer.RegisterCollectable(registry);
  // the more publishers are being collected the higher the prob to see txs
  // which are approved by different tx
  _families = buildHistogramsMap(
      registry, {{NUM_PUBLISHERS, std::to_string(_zmqPublishers.size())}});

  analyzeBlowballsPeriodically();
  populateApproversDBs();
}

void TopologyMapper::analyzeBlowballsPeriodically() {
  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();

  auto& thisRef = *this;
  if (_snapshotInterval > 0) {
    pubWorker.schedule_periodically(
        pubThread.now(), std::chrono::seconds(_snapshotInterval),
        [&thisRef](auto scbl) { thisRef.analyzeBlowballs(); });
  }
}

void TopologyMapper::analyzeBlowballs() {
  std::vector<double> buckets(200);
  double currInterval = 0;
  std::generate(buckets.begin(), buckets.end(),
                [&currInterval]() { return ++currInterval; });
  // run over all different dbs and store for each tx
  // its approvers
  std::map<std::string, std::set<std::string>> txToApproversSet;
  for (auto& urlDBPair : _urlToDB) {
    auto& db = urlDBPair.second;
    auto approversMap = db.swapDB();

    for (auto& hashApproverPair : *approversMap) {
      auto& approversSet = txToApproversSet[hashApproverPair.first];
      approversSet.insert(hashApproverPair.second);
    }
  }

  for (auto& kv : txToApproversSet) {
    _families.at(TX_NUM_APPROVERS)
        .get()
        .Add({}, std::move(buckets))
        .Observe(kv.second.size());
  }
}

void TopologyMapper::populateApproversDBs() {
  auto zmqThread = rxcpp::schedulers::make_new_thread();

  auto& thisRef = *this;
  std::vector<pplx::task<void>> observableTasks;
  for (auto& kv : _urlToZmqObservables) {
    auto zmqURL = kv.first;
    auto task = pplx::task<void>([ zmqURL = std::move(zmqURL), &thisRef ]() {
      thisRef.loadApproversDB(std::move(zmqURL));
    });
    observableTasks.push_back(std::move(task));
  }

  std::for_each(observableTasks.begin(), observableTasks.end(),
                [&](pplx::task<void> task) { task.get(); });
}

void TopologyMapper::loadApproversDB(std::string zmqURL) {
  auto& db = (*_urlToDB.find(zmqURL)).second;
  auto& zmqObservable = _urlToZmqObservables[zmqURL];
  zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .subscribe(
          [&db](std::shared_ptr<iri::IRIMessage> msg) {

            // assuming no tx is a milestone
            if (msg->type() != iri::IRIMessageType::TX) return;

            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));
            db.put(tx->trunk(), tx->hash());
            db.put(tx->branch(), tx->hash());
          },
          []() {});
}

using namespace prometheus;

PrometheusCollector::HistogramsMap TopologyMapper::buildHistogramsMap(
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