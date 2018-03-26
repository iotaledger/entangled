#include <atomic>
#include <iostream>
#include <map>
#include <set>
#include <cstring>
#include <list>
#include <unordered_set>
#include <glog/logging.h>
#include <ccurl/ccurl.h>
#include <iota/utils/common/zmqpub.hpp>
#include <iota/utils/common/txauxiliary.hpp>
#include <prometheus/exposer.h>
#include "utils/common/tangledb.hpp"
#include <iota/utils/common/txauxiliary.hpp>
#include <prometheus/exposer.h>
#include <iota/utils/echocatcher/echocatcher.hpp>
#include "utils/common/tangledb.hpp"

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
  if (!StatsExposer::parseConfiguration(conf)) {
    return false;
  }

  if (conf["iri_host"] && conf["publishers"] && conf["mwm"] &&
      conf["tangledb_warmup_period"] && conf["prometheus_exposer_uri"]) {
    _iriHost = conf["iri_host"].as<std::string>();
    _zmqPublishers = conf["publishers"].as<std::list<std::string>>();
    _mwm = conf["mwm"].as<uint32_t>();
    _tangleDBWarmupPeriod = conf["tangledb_warmup_period"].as<uint32_t>();
    _prometheusExpURI = conf["prometheus_exposer_uri"].as<std::string>();
    return true;
  }

  return false;
}

void EchoCatcher::expose() {
  LOG(INFO) << __FUNCTION__;
  ccurl_pow_init();

  _iriClient = std::make_shared<api::IRIClient>(_iriHost);
  _zmqObservable = rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
          [&](auto s) { zmqPublisher(std::move(s), *_zmqPublishers.begin()); });

  loadDB();
  // TODO: should get arg for interval between following TXs and update a
  // map<hash,<hashed,start_time>>
  auto hashed = broadcastTransactions();
  auto start = std::chrono::system_clock::now();

  handleRecievedTransactions(hashed, start);
}

void EchoCatcher::loadDB() {
  auto tangleDBWarmupPeriod =
      std::chrono::milliseconds(_tangleDBWarmupPeriod * 1000);

  auto zmqThread = rxcpp::schedulers::make_new_thread();

  const auto& stopPopulatingDBTime =
      std::chrono::system_clock::now() + tangleDBWarmupPeriod;

  _zmqObservable.observe_on(rxcpp::synchronize_new_thread())
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

EchoCatcher::HashedTX EchoCatcher::broadcastTransactions() {
  auto task =
      _iriClient->getTransactionsToApprove()
          .then(fillTX)
          .then([*this](std::string tx) { return powTX(std::move(tx), _mwm); })
          .then(hashTX);

  task.wait();
  auto hashed = task.get();
  LOG(INFO) << "Hash: " << hashed.hash;

  auto broadcast = _iriClient->broadcastTransactions({hashed.tx});
  return std::move(hashed);
}

void EchoCatcher::handleRecievedTransactions(
    EchoCatcher::HashedTX hashed,
    std::chrono::time_point<std::chrono::system_clock> start) {
  // split to prometheus set up and actual listening
  using namespace prometheus;
  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  auto zmqURL = *_zmqPublishers.begin();

  auto& gauge_received_family =
      BuildGauge()
          .Name("time_elapsed_received")
          .Help(
              "#Milli seconds it took for tx to travel back to transaction's "
              "original source(\"listen_node\")")
          .Labels({{"publish_node", zmqURL}, {"listen_node", _iriHost}})
          .Register(*registry);

  auto& gauge_arrived_family =
      BuildGauge()
          .Name("time_elapsed_arrived")
          .Help(
              "#Milliseconds it took for tx to arrive to destination "
              "(\"publish_node\")")
          .Labels({{"publish_node", zmqURL}, {"listen_node", _iriHost}})
          .Register(*registry);

  exposer.RegisterCollectable(registry);

  auto zmqThread = rxcpp::schedulers::make_new_thread();
  std::atomic<bool> haveAllTXReturned = false;

  cuckoohash_map<std::string, std::chrono::system_clock::time_point>
      hashToDiscoveryTimestamp;
  std::vector<pplx::task<void>> handleTXtasks;

  _zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .subscribe(
          [*this, start, hashed, &gauge_received_family, &gauge_arrived_family,
           &haveAllTXReturned, &hashToDiscoveryTimestamp,
           &handleTXtasks](std::shared_ptr<iri::IRIMessage> msg) {
            if (msg->type() != iri::IRIMessageType::TX) return;

            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));

            auto received = std::chrono::system_clock::now();

            auto txHandlerTask = txAuxiliary::handleUnseenTransactions(
                tx, hashToDiscoveryTimestamp, received, _iriClient);
            handleTXtasks.push_back(std::move(txHandlerTask));

            if (tx->hash() == hashed.hash) {
              auto& current_received_duration_gauge = gauge_received_family.Add(
                  {{"bundle_size", std::to_string(tx->lastIndex() + 1)}});

              auto& current_arrived_duration_gauge = gauge_arrived_family.Add(
                  {{"bundle_size", std::to_string(tx->lastIndex() + 1)}});

              auto elapsed_until_received =
                  std::chrono::duration_cast<std::chrono::milliseconds>(
                      received - start)
                      .count();

              auto elapsed_until_arrived =
                  std::chrono::duration_cast<std::chrono::milliseconds>(
                      tx->arrivalTime() - start)
                      .count();

              current_received_duration_gauge.Set(elapsed_until_received);
              current_arrived_duration_gauge.Set(elapsed_until_arrived);
              haveAllTXReturned = true;
            }
          },
          []() {});

  std::for_each(handleTXtasks.begin(), handleTXtasks.end(),
                [&](pplx::task<void> task) { task.get(); });
}
