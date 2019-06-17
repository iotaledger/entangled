#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <unordered_set>

#include <glog/logging.h>
#include <prometheus/exposer.h>

#include "tanglescope/broadcastrecievecollecter.hpp"
#include "tanglescope/common/tangledb.hpp"
#include "tanglescope/common/txauxiliary.hpp"
#include "tanglescope/common/zmqpub.hpp"
#include "tanglescope/echocollector.hpp"

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

bool EchoCollector::parseConfiguration(const YAML::Node& conf) {
  if (!BroadcastReceiveCollector::parseConfiguration(conf)) {
    return false;
  }
  if (conf[DISCOVERY_INTERVAL]) {
    _discoveryInterval = conf[DISCOVERY_INTERVAL].as<uint32_t>();
    return true;
  }
  return false;
}

using namespace prometheus;

void EchoCollector::subscribeToTransactions(std::string zmqURL,
                                            const BroadcastReceiveCollector::ZmqObservable& zmqObservable,
                                            std::shared_ptr<Registry> registry) {
  std::atomic<bool> haveAllTXReturned = false;
  auto histograms = buildHistogramsMap(registry, "echocollector", {{"listen_node", _iriHost}, {"zmq_url", zmqURL}},
                                       nameToDescHistogram);

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
              auto lmhs = std::static_pointer_cast<iri::LMHSMessage>(std::move(msg));
              _latestSolidMilestoneHash = lmhs->latestSolidMilestoneHash();
              return;
            }

            if (msg->type() != iri::IRIMessageType::TX || _latestSolidMilestoneHash.empty()) return;

            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));
            auto received = std::chrono::system_clock::now();
            auto task = handleUnseenTransactions(tx, received, histograms, buckets);

            tasks.push_back(std::move(task));

            BroadcastReceiveCollector::BroadcastInfo bi;
            if (_hashToBroadcastTime.find(tx->hash(), bi)) {
              auto elapsedUntilReceived =
                  std::chrono::duration_cast<std::chrono::milliseconds>(received - bi.tp).count();
              auto elapsedUntilArrived =
                  std::chrono::duration_cast<std::chrono::milliseconds>(tx->arrivalTime() - bi.tp).count();

              histograms.at("time_elapsed_received")
                  .get()
                  .Add({{"bundle_size", std::to_string(tx->lastIndex() + 1)}}, buckets)
                  .Observe(elapsedUntilReceived);
              histograms.at("time_elapsed_arrived")
                  .get()
                  .Add({{"bundle_size", std::to_string(tx->lastIndex() + 1)}}, buckets)
                  .Observe(elapsedUntilArrived);

              if (_broadcastInterval == 0) {
                haveAllTXReturned = true;
              }
            }
          },
          []() {});

  std::for_each(tasks.begin(), tasks.end(), [&](auto& task) { task.get(); });
}

boost::future<void> EchoCollector::handleUnseenTransactions(std::shared_ptr<iri::TXMessage> tx,
                                                            std::chrono::time_point<std::chrono::system_clock> received,
                                                            HistogramsMap& histograms,
                                                            const std::vector<double>& buckets) {
  static std::atomic<std::chrono::time_point<std::chrono::system_clock>> lastDiscoveryTime = received;
  std::chrono::system_clock::time_point txTime;

  if (_hashToDiscoveryTime.find(tx->hash(), txTime)) {
    auto txArrivalLatency = std::chrono::duration_cast<std::chrono::milliseconds>(received - txTime).count();
    _hashToDiscoveryTime.erase(tx->hash());

    histograms.at("time_elapsed_unseen_tx_published")
        .get()
        .Add({{"bundle_size", std::to_string(tx->lastIndex() + 1)}}, buckets)
        .Observe(txArrivalLatency);
  } else {
    auto elapsedFromLastTime =
        std::chrono::duration_cast<std::chrono::seconds>(received - lastDiscoveryTime.load()).count();
    if (elapsedFromLastTime > _discoveryInterval) {
      lastDiscoveryTime = received;
      std::shared_lock<std::shared_mutex> lock(_milestoneMutex);
      auto lmhs = _latestSolidMilestoneHash;
      return txAuxiliary::handleUnseenTransactions(std::move(tx), _hashToDiscoveryTime, std::move(received), _api,
                                                   std::move(lmhs));
    }
  }

  return {};
}
