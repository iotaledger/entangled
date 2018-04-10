#include <ccurl/ccurl.h>
#include <glog/logging.h>
#include <prometheus/exposer.h>
#include <algorithm>
#include <atomic>
#include <chrono>
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

#include "common/trinary/tryte_long.h"

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
  using namespace std::chrono;
  std::string tx = TX_TRYTES;

  tx.replace(2430, 81, response.trunkTransaction);
  tx.replace(2511, 81, response.branchTransaction);
  auto timestampSeconds =
      duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
  tryte_t trytes[10] = "999999999";
  long_to_trytes(timestampSeconds, trytes);
  tx.replace(2322, 9, (char*)trytes);
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

  if (conf[IRI_HOST] && conf[PUBLISHERS] && conf[MWM] &&
      conf[TANGLE_DB_WARMUP_TIME] && conf[BROADCAST_INTERVAL] &&
      conf[DISCOVERY_INTERVAL]) {
    _iriHost = conf[IRI_HOST].as<std::string>();
    _zmqPublishers = conf[PUBLISHERS].as<std::list<std::string>>();
    _mwm = conf[MWM].as<uint32_t>();
    _tangleDBWarmupPeriod = conf[TANGLE_DB_WARMUP_TIME].as<uint32_t>();
    _broadcastInterval = conf[BROADCAST_INTERVAL].as<uint32_t>();
    _discoveryInterval = conf[DISCOVERY_INTERVAL].as<uint32_t>();
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
            [&](auto s) { zmqPublisher(std::move(s), url); });
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
  auto getTransactionsTask =
      _iriClient->getTransactionsToApprove()
          .then(fillTX)
          .then([this](std::string tx) { return powTX(std::move(tx), _mwm); })
          .then(hashTX);

  try {
    auto hashed = getTransactionsTask.get();
    LOG(INFO) << "Hash: " << hashed.hash;
    auto broadcastTask = _iriClient->broadcastTransactions({hashed.tx});
    _hashToBroadcastTime.insert(hashed.hash, std::chrono::system_clock::now());
    broadcastTask.wait();
  } catch (const std::exception& e) {
    LOG(ERROR) << __FUNCTION__ << " Exception: " << e.what();
  }
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
  auto families = buildHistogramsMap(
      registry, {{"listen_node", _iriHost}, {"zmq_url", zmqURL}});

  std::vector<pplx::task<void>> tasks;
  std::vector<double> buckets(400);
  double currInterval = 0;
  std::generate(buckets.begin(), buckets.end(), [&currInterval]() {
    currInterval += BUCKET_WIDTH;
    return currInterval;
  });

  zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .subscribe(
          [&](std::shared_ptr<iri::IRIMessage> msg) {

            if (msg->type() == iri::IRIMessageType::LMHS) {
              std::unique_lock<std::shared_mutex> lock(_milestoneMutex);
              auto lmhs =
                  std::static_pointer_cast<iri::LMHSMessage>(std::move(msg));
              _latestSolidMilestoneHash = lmhs->latestSolidMilestoneHash();
              return;
            }

            if (msg->type() != iri::IRIMessageType::TX ||
                _latestSolidMilestoneHash.empty())
              return;

            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));
            auto received = std::chrono::system_clock::now();
            auto task =
                handleUnseenTransactions(tx, received, families, buckets);

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
                  .Add({{"bundle_size", std::to_string(tx->lastIndex() + 1)}},
                       buckets)
                  .Observe(elapsedUntilReceived);
              families.at("time_elapsed_arrived")
                  .get()
                  .Add({{"bundle_size", std::to_string(tx->lastIndex() + 1)}},
                       buckets)
                  .Observe(elapsedUntilArrived);

              if (_broadcastInterval == 0) {
                haveAllTXReturned = true;
              }
            }
          },
          []() {});

  std::for_each(tasks.begin(), tasks.end(),
                [&](pplx::task<void> task) { task.get(); });
}

pplx::task<void> EchoCatcher::handleUnseenTransactions(
    std::shared_ptr<iri::TXMessage> tx,
    std::chrono::time_point<std::chrono::system_clock> received,
    HistogramsMap& families, const std::vector<double>& buckets) {
  static std::atomic<std::chrono::time_point<std::chrono::system_clock>>
      lastDiscoveryTime = received;
  std::chrono::system_clock::time_point txTime;

  if (_hashToDiscoveryTime.find(tx->hash(), txTime)) {
    auto txArrivalLatency =
        std::chrono::duration_cast<std::chrono::milliseconds>(received - txTime)
            .count();
    _hashToDiscoveryTime.erase(tx->hash());

    families.at("time_elapsed_unseen_tx_published")
        .get()
        .Add({{"bundle_size", std::to_string(tx->lastIndex() + 1)}}, buckets)
        .Observe(txArrivalLatency);
  } else {
    auto elapsedFromLastTime = std::chrono::duration_cast<std::chrono::seconds>(
                                   received - lastDiscoveryTime.load())
                                   .count();
    if (elapsedFromLastTime > _discoveryInterval) {
      lastDiscoveryTime = received;
      std::shared_lock<std::shared_mutex> lock(_milestoneMutex);
      auto lmhs = _latestSolidMilestoneHash;
      return txAuxiliary::handleUnseenTransactions(
          std::move(tx), _hashToDiscoveryTime, std::move(received), _iriClient,
          std::move(lmhs));
    }
  }
  return pplx::create_task([]() {});
}

PrometheusCollector::HistogramsMap EchoCatcher::buildHistogramsMap(
    std::shared_ptr<Registry> registry,
    const std::map<std::string, std::string>& labels) {
  static std::map<std::string, std::string> nameToDesc = {
      {"time_elapsed_received",
       "#Milliseconds it took for tx to travel back to transaction's "
       "original source(\"listen_node\") [each interval is " +
           std::to_string(BUCKET_WIDTH) + " milliseconds]"},
      {"time_elapsed_arrived",
       "#Milliseconds it took for tx to arrive to destination "
       "(\"publish_node\") [each interval is " +
           std::to_string(BUCKET_WIDTH) + " milliseconds]"},
      {"time_elapsed_unseen_tx_published",
       "#Milliseconds it took for tx to be published since the moment "
       "client "
       "first learned about it [each interval is " +
           std::to_string(BUCKET_WIDTH) + " milliseconds]"}};

  std::map<std::string, std::reference_wrapper<Family<Histogram>>> famillies;
  for (const auto& kv : nameToDesc) {
    auto& curr_family = BuildHistogram()
                            .Name("echocatcher_" + kv.first)
                            .Help(kv.second)
                            .Labels(labels)
                            .Register(*registry);

    famillies.insert(
        std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return std::move(famillies);
}