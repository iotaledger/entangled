#include <glog/logging.h>
#include <prometheus/exposer.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <iota/tanglescope/common/tangledb.hpp>
#include <iota/tanglescope/common/txauxiliary.hpp>
#include <iota/tanglescope/common/zmqpub.hpp>
#include <iota/tanglescope/echocollector/echocollector.hpp>
#include <list>
#include <map>
#include <set>
#include <unordered_set>

#include "common/helpers/digest.h"
#include "common/helpers/pow.h"
#include "common/trinary/tryte_long.h"

constexpr static auto DEPTH = 3;

using namespace iota::tanglescope;

std::map<std::string, std::string> EchoCollector::nameToDescHistogram = {
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

std::string fillTX(
    const cppclient::GetTransactionsToApproveResponse& response) {
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

std::string powTX(std::string tx, int mwm) {
  char* foundNonce = iota_pow(tx.data(), mwm);
  tx.replace(2646, 27, foundNonce);
  free(foundNonce);

  return tx.data();
}

EchoCollector::HashedTX hashTX(std::string tx) {
  char* digest = iota_digest(tx.data());
  EchoCollector::HashedTX hashed = {digest, std::move(tx)};
  free(digest);
  return std::move(hashed);
}

bool EchoCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (conf[IRI_HOST] && conf[IRI_PORT] && conf[PUBLISHERS] && conf[MWM] &&
      conf[BROADCAST_INTERVAL] && conf[DISCOVERY_INTERVAL]) {
    _iriHost = conf[IRI_HOST].as<std::string>();
    _iriPort = conf[IRI_PORT].as<uint32_t>();
    _zmqPublishers = conf[PUBLISHERS].as<std::list<std::string>>();
    _mwm = conf[MWM].as<uint32_t>();
    _broadcastInterval = conf[BROADCAST_INTERVAL].as<uint32_t>();
    _discoveryInterval = conf[DISCOVERY_INTERVAL].as<uint32_t>();
    return true;
  }

  return false;
}

void EchoCollector::collect() {
  LOG(INFO) << __FUNCTION__;

  _api = std::make_shared<cppclient::BeastIotaAPI>(_iriHost, _iriPort);

  for (const auto& url : _zmqPublishers) {
    auto zmqObservable =
        rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
            [&](auto s) { zmqPublisher(std::move(s), url); });
    _urlToZmqObservables.insert(std::pair(url, zmqObservable));
  }

  broadcastTransactions();
  handleReceivedTransactions();
}

void EchoCollector::broadcastTransactions() {
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
void EchoCollector::broadcastOneTransaction() {
  auto hashTXFuture =
      boost::async(boost::launch::async,
                   [this] { return _api->getTransactionsToApprove(DEPTH); })
          .then([](auto resp) { return fillTX(resp.get()); })
          .then(
              [this](auto resp) { return powTX(std::move(resp.get()), _mwm); })
          .then([](auto resp) { return hashTX(resp.get()); });

  try {
    auto hashed = hashTXFuture.get();
    LOG(INFO) << "Hash: " << hashed.hash;

    auto broadcastFuture = boost::async(boost::launch::async, [hashed, this] {
      return _api->broadcastTransactions({hashed.tx});
    });

    _hashToBroadcastTime.insert(hashed.hash, std::chrono::system_clock::now());
    broadcastFuture.wait();
  } catch (const std::exception& e) {
    LOG(ERROR) << __FUNCTION__ << " Exception: " << e.what();
  }
}

void EchoCollector::handleReceivedTransactions() {
  using namespace prometheus;
  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  exposer.RegisterCollectable(registry);
  auto zmqThread = rxcpp::schedulers::make_new_thread();

  auto& catcher = *this;
  std::vector<boost::future<void>> observableTasks;
  for (auto& kv : _urlToZmqObservables) {
    auto zmqURL = kv.first;
    auto zmqObservable = kv.second;
    auto task = boost::async(
        boost::launch::async,
        [zmqURL = std::move(zmqURL), &zmqObservable, &registry, &catcher]() {
          catcher.subscribeToTransactions(zmqURL, zmqObservable, registry);
        });
    observableTasks.push_back(std::move(task));
  }

  std::for_each(observableTasks.begin(), observableTasks.end(),
                [&](auto& task) { task.wait(); });
}

using namespace prometheus;

void EchoCollector::subscribeToTransactions(
    std::string zmqURL, const EchoCollector::ZmqObservable& zmqObservable,
    std::shared_ptr<Registry> registry) {
  std::atomic<bool> haveAllTXReturned = false;
  auto histograms = buildHistogramsMap(
      registry, "echocollector",
      {{"listen_node", _iriHost}, {"zmq_url", zmqURL}}, nameToDescHistogram);

  std::vector<boost::future<void>> tasks;
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
                handleUnseenTransactions(tx, received, histograms, buckets);

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

              histograms.at("time_elapsed_received")
                  .get()
                  .Add({{"bundle_size", std::to_string(tx->lastIndex() + 1)}},
                       buckets)
                  .Observe(elapsedUntilReceived);
              histograms.at("time_elapsed_arrived")
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

  std::for_each(tasks.begin(), tasks.end(), [&](auto& task) { task.get(); });
}

boost::future<void> EchoCollector::handleUnseenTransactions(
    std::shared_ptr<iri::TXMessage> tx,
    std::chrono::time_point<std::chrono::system_clock> received,
    HistogramsMap& histograms, const std::vector<double>& buckets) {
  static std::atomic<std::chrono::time_point<std::chrono::system_clock>>
      lastDiscoveryTime = received;
  std::chrono::system_clock::time_point txTime;

  if (_hashToDiscoveryTime.find(tx->hash(), txTime)) {
    auto txArrivalLatency =
        std::chrono::duration_cast<std::chrono::milliseconds>(received - txTime)
            .count();
    _hashToDiscoveryTime.erase(tx->hash());

    histograms.at("time_elapsed_unseen_tx_published")
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
          std::move(tx), _hashToDiscoveryTime, std::move(received), _api,
          std::move(lmhs));
    }
  }

  return {};
}
