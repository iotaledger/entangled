#include <ccurl/ccurl.h>
#include <glog/logging.h>
#include <prometheus/exposer.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include <iota/utils/common/tangledb.hpp>
#include <iota/utils/common/txauxiliary.hpp>
#include <iota/utils/common/zmqpub.hpp>
#include <iota/utils/echocatcher/echocatcher.hpp>
#include <list>
#include <map>
#include <set>
#include <unordered_set>

using namespace iota::utils;

const std::string TX_TRYTES =
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999SAYHELLOTOECHOCATCHERECHOCATCHING"
    "SINCETWENTYSEVENTEENONTHEIOTATANGLE999999999999999999999999999999999999999"
    "9VD9999999999999999999999999JAKBHNJIE999999999999999999JURSJVFIECKJYEHPATC"
    "XADQGHABKOOEZCRUHLIDHPNPIGRCXBFBWVISWCF9ODWQKLXBKY9FACCKVXRAGZ999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "999999999";

std::string fillTX(api::GetTransactionsToApproveResponse response) {
  std::string tx = TX_TRYTES;

  tx.replace(2430, 81, response.trunkTransaction);
  tx.replace(2511, 81, response.branchTransaction);

  return std::move(tx);
}

std::string powTX(std::string tx, int mwm) { return ccurl_pow(tx.data(), mwm); }

EchoCatcher::HashedTX hashTX(std::string tx) {
  return {ccurl_digest_transaction(tx.data()), std::move(tx)};
}

bool EchoCatcher::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (conf["iri_host"] && conf["publishers"] && conf["mwm"] &&
      conf["tangledb_warmup_period"] && conf["broadcast_interval"]) {
    _iriHost = conf["iri_host"].as<std::string>();
    _zmqPublishers = conf["publishers"].as<std::list<std::string>>();
    _mwm = conf["mwm"].as<uint32_t>();
    _tangleDBWarmupPeriod = conf["tangledb_warmup_period"].as<uint32_t>();
    _broadcastInterval = conf["broadcast_interval"].as<uint32_t>();
    return true;
  }

  return false;
}

void EchoCatcher::collect() {
  LOG(INFO) << __FUNCTION__;
  ccurl_pow_init();

  _iriClient = std::make_shared<api::IRIClient>(_iriHost);

  for (const auto& url : _zmqPublishers) {
    auto zmqObservable =
        rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
            [&](auto s) {
              zmqPublisher(std::move(s), *_zmqPublishers.begin());
            });
    _urlToZmqObservables.insert(std::pair(url, zmqObservable));
  }

  loadDB();
  broadcastTransactions();
  handleReceivedTransactions();
}

void EchoCatcher::loadDB() {
  auto tangleDBWarmupPeriod =
      std::chrono::milliseconds(_tangleDBWarmupPeriod * 1000);

  auto zmqThread = rxcpp::schedulers::make_new_thread();

  const auto& stopPopulatingDBTime =
      std::chrono::system_clock::now() + tangleDBWarmupPeriod;

  // can load using only one zmq
  auto zmqObservable = (*_urlToZmqObservables.begin()).second;
  zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .take_while([stopPopulatingDBTime](std::shared_ptr<iri::IRIMessage> msg) {
        return std::chrono::system_clock::now() < stopPopulatingDBTime;
      })
      .subscribe(
          [](std::shared_ptr<iri::IRIMessage> msg) {
            if (msg->type() != iri::IRIMessageType::TX) return;

            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));

            TangleDB::TXRecord txRec = {tx->hash(), tx->trunk(), tx->branch()};
            TangleDB::instance().put(std::move(txRec));
          },
          []() {});
}

void EchoCatcher::broadcastTransactions() {
  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();

  if (_broadcastInterval > 0) {
    pubWorker.schedule_periodically(
        pubThread.now(), std::chrono::seconds(_broadcastInterval),
        [&](auto scbl) { broadcastOneTransaction(); });
  } else {
    broadcastOneTransaction();
  }
}
void EchoCatcher::broadcastOneTransaction() {
  auto task =
      _iriClient->getTransactionsToApprove()
          .then(fillTX)
          .then([*this](std::string tx) { return powTX(std::move(tx), _mwm); })
          .then(hashTX);

  task.wait();
  auto hashed = task.get();
  LOG(INFO) << "Hash: " << hashed.hash;

  _iriClient->broadcastTransactions({hashed.tx});
  _hashToBroadcastTime.insert(hashed.tx, std::chrono::system_clock::now());
}

void EchoCatcher::handleReceivedTransactions() {
  using namespace prometheus;
  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  exposer.RegisterCollectable(registry);
  auto zmqThread = rxcpp::schedulers::make_new_thread();

  auto& catcher = *this;
  std::vector<pplx::task<void>> observableTasks;
  for (auto& kv : _urlToZmqObservables) {
    auto zmqURL = kv.first;
    auto zmqObservable = kv.second;
    auto task = pplx::task<void>([
      zmqURL = std::move(zmqURL), &zmqObservable, &registry, &catcher
    ]() { catcher.subscribeToTransactions(zmqURL, zmqObservable, registry); });
    observableTasks.push_back(std::move(task));
  }

  std::for_each(observableTasks.begin(), observableTasks.end(),
                [&](pplx::task<void> task) { task.get(); });
}

using namespace prometheus;

void EchoCatcher::subscribeToTransactions(
    std::string zmqURL, const EchoCatcher::ZmqObservable& zmqObservable,
    std::shared_ptr<Registry> registry) {
  std::atomic<bool> haveAllTXReturned = false;
  auto families = buildMetricsMap(
      registry, {{"listen_node", _iriHost}, {"zmq_url", zmqURL}});
  std::vector<pplx::task<void>> tasks;

  zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .subscribe(
          [&](std::shared_ptr<iri::IRIMessage> msg) {
            if (msg->type() != iri::IRIMessageType::TX) return;
            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));
            auto received = std::chrono::system_clock::now();
            auto& timeUntilPublishedGauge =
                families.at("time_elapsed_unseen_tx_published")
                    .get()
                    .Add(
                        {{"bundle_size", std::to_string(tx->lastIndex() + 1)}});
            auto task = pplx::task<void>([&]() {
              handleUnseenTransactions(tx, _hashToDiscoveryTime, received,
                                       _iriClient, timeUntilPublishedGauge);
            });
            tasks.push_back(std::move(task));

            std::chrono::system_clock::time_point broadcastTime;
            if (_hashToBroadcastTime.find(tx->hash(), broadcastTime)) {
              auto elapsedUntilReceived =
                  std::chrono::duration_cast<std::chrono::milliseconds>(
                      received - broadcastTime)
                      .count();
              auto elapsedUntilArrived =
                  std::chrono::duration_cast<std::chrono::milliseconds>(
                      tx->arrivalTime() - broadcastTime)
                      .count();

              families.at("time_elapsed_received")
                  .get()
                  .Add({{"bundle_size", std::to_string(tx->lastIndex() + 1)}})
                  .Set(elapsedUntilReceived);
              families.at("time_elapsed_arrived")
                  .get()
                  .Add({{"bundle_size", std::to_string(tx->lastIndex() + 1)}})
                  .Set(elapsedUntilArrived);

              if (_broadcastInterval == 0) {
                haveAllTXReturned = true;
              }
            }
          },
          []() {});

  std::for_each(tasks.begin(), tasks.end(),
                [&](pplx::task<void> task) { task.get(); });
}

void EchoCatcher::handleUnseenTransactions(
    std::shared_ptr<iri::TXMessage> tx,
    cuckoohash_map<std::string, std::chrono::system_clock::time_point>&
        hashToSeenTimestamp,
    std::chrono::time_point<std::chrono::system_clock> received,
    std::weak_ptr<api::IRIClient> iriClient, Gauge& timeUntilPublishedGauge) {
  auto txHandlerTask = txAuxiliary::handleUnseenTransactions(
      tx, _hashToDiscoveryTime, received, iriClient);

  if (txHandlerTask.get().has_value()) {
    timeUntilPublishedGauge.Set(txHandlerTask.get().value());
  }
}

EchoCatcher::GaugeMap EchoCatcher::buildMetricsMap(
    std::shared_ptr<Registry> registry,
    const std::map<std::string, std::string>& labels) {
  static std::map<std::string, std::string> nameToDesc = {
      {"time_elapsed_received",
       "#Milliseconds it took for tx to travel back to transaction's "
       "original source(\"listen_node\")"},
      {"time_elapsed_arrived",
       "#Milliseconds it took for tx to arrive to destination "
       "(\"publish_node\")"},
      {"time_elapsed_unseen_tx_published",
       "#Milliseconds it took for tx to be published since the moment client "
       "first learned about it"}};

  std::map<std::string, std::reference_wrapper<Family<Gauge>>> famillies;
  for (const auto& kv : nameToDesc) {
    auto& curr_family = BuildGauge()
                            .Name("echocatcher_" + kv.first)
                            .Help(kv.second)
                            .Labels(labels)
                            .Register(*registry);

    famillies.insert(
        std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return std::move(famillies);
}
