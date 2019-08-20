#include <glog/logging.h>
#include <prometheus/exposer.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <iota/models/bundle.hpp>
#include <iota/models/transaction.hpp>
#include <list>
#include <map>
#include <set>
#include <unordered_set>

#include "tanglescope/broadcastrecievecollecter.hpp"
#include "tanglescope/common/tangledb.hpp"
#include "tanglescope/common/txauxiliary.hpp"
#include "tanglescope/common/zmqpub.hpp"

constexpr static auto DEPTH = 3;

using namespace iota::tanglescope;

bool BroadcastReceiveCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (conf[IRI_HOST] && conf[IRI_PORT] && conf[PUBLISHERS] && conf[MWM] && conf[BROADCAST_INTERVAL]) {
    _iriHost = conf[IRI_HOST].as<std::string>();
    _iriPort = conf[IRI_PORT].as<uint32_t>();
    _zmqPublishers = conf[PUBLISHERS].as<std::list<std::string>>();
    _mwm = conf[MWM].as<uint32_t>();
    _broadcastInterval = conf[BROADCAST_INTERVAL].as<uint32_t>();
    return true;
  }

  return false;
}

void BroadcastReceiveCollector::collect() {
  LOG(INFO) << __FUNCTION__;

  _api = std::make_shared<cppclient::BeastIotaAPI>(_iriHost, _iriPort);

  for (const auto& url : _zmqPublishers) {
    if (urlToZmqObservables.find(url) == urlToZmqObservables.end()) {
      urlToZmqObservables[url] = rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
          [&](auto s) { zmqPublisher(std::move(s), url); });
    }
  }

  doPeriodically();
  broadcastTransactions();
  receivedTransactions();
}

void BroadcastReceiveCollector::broadcastTransactions() {
  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();

  if (_broadcastInterval > 0) {
    pubWorker.schedule_periodically(pubThread.now(), std::chrono::seconds(_broadcastInterval),
                                    [&](auto) { broadcastOneTransaction(); });
  } else {
    broadcastOneTransaction();
  }
}
void BroadcastReceiveCollector::broadcastOneTransaction() {
  using namespace txAuxiliary;
  using namespace std::chrono;

  system_clock::time_point t1 = system_clock::now();
  auto hashTXFuture = boost::async(boost::launch::async, [this] { return _api->getTransactionsToApprove(DEPTH); })
                          .then(fillTX)
                          .then([this](auto resp) { return powTX(std::move(resp.get()), _mwm); })
                          .then(hashTX);

  try {
    auto hashed = hashTXFuture.get();
    LOG(INFO) << "Hash: " << hashed.hash;
    artificialyDelay();
    system_clock::time_point t2 = system_clock::now();
    auto duration = duration_cast<milliseconds>(t2 - t1).count();
    _hashToBroadcastTime.insert(hashed.hash,
                                BroadcastInfo{std::chrono::system_clock::now(), static_cast<uint64_t>(duration)});

    auto storeFuture =
        boost::async(boost::launch::async, [hashed, this] { return _api->storeTransactions({hashed.tx}); });
    storeFuture.wait();
    auto broadcastFuture =
        boost::async(boost::launch::async, [hashed, this] { return _api->broadcastTransactions({hashed.tx}); });

    broadcastFuture.wait();
  } catch (const std::exception& e) {
    LOG(ERROR) << __FUNCTION__ << " Exception: " << e.what();
  }
}

void BroadcastReceiveCollector::receivedTransactions() {
  using namespace prometheus;
  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  exposer.RegisterCollectable(registry);

  auto& catcher = *this;
  std::vector<boost::future<void>> observableTasks;
  for (const auto& url : _zmqPublishers) {
    auto zmqURL = url;
    auto zmqObservable = urlToZmqObservables[url];
    auto task = boost::async(boost::launch::async, [zmqURL = std::move(zmqURL), &zmqObservable, &registry, &catcher]() {
      catcher.subscribeToTransactions(zmqURL, zmqObservable, registry);
    });
    observableTasks.push_back(std::move(task));
  }

  std::for_each(observableTasks.begin(), observableTasks.end(), [&](auto& task) { task.wait(); });
}
