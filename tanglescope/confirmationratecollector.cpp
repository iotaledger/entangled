#include "tanglescope/confirmationratecollector.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

#include <glog/logging.h>
#include <prometheus/exposer.h>

#include "tanglescope/common/tangledb.hpp"
#include "tanglescope/common/txauxiliary.hpp"
#include "tanglescope/common/zmqpub.hpp"
#include "tanglescope/confirmationratecollector.hpp"

using namespace iota::tanglescope;

std::map<std::string, std::string> CRCollector::nameToDescGauges = {
    {CRCollector::CONFIRMATION_RATE_ZMQ,
     "Confirmation rate ratio [0,1] as it is perceived by inspecting the zmq "
     "stream"},
    {CRCollector::CONFIRMATION_RATE_API,
     "Confirmation rate ratio [0,1] as it is perceived by making api calls to "
     "\"getInclusionStates\""}};

bool CRCollector::parseConfiguration(const YAML::Node& conf) {
  if (!BroadcastReceiveCollector::parseConfiguration(conf)) {
    return false;
  }
  if (conf[MESAUREMENT_LOWER_BOUND] && conf[MESAUREMENT_UPPER_BOUND] && conf[ENABLE_CR_FROM_API] &&
      conf[ADDITIONAL_LATENCY_STEP_SECONDS] && conf[ADDITIONAL_LATENCY_NUM_STEPS]) {
    _measurementUpperBound = conf[MESAUREMENT_UPPER_BOUND].as<uint32_t>();
    _measurementLowerBound = conf[MESAUREMENT_LOWER_BOUND].as<uint32_t>();
    _enableApi = conf[ENABLE_CR_FROM_API].as<bool>();
    _addtionalLatencyStepSeconds = std::chrono::seconds(conf[ADDITIONAL_LATENCY_STEP_SECONDS].as<uint32_t>());
    _addtionalLatencyNumSteps = conf[ADDITIONAL_LATENCY_NUM_STEPS].as<uint32_t>();
    return true;
  }
  return false;
}

void CRCollector::doPeriodically() {
  if (!_enableApi) return;
  _collectorThread = rxcpp::schedulers::make_new_thread();
  _collectorWorker = _collectorThread.create_worker();

  _collectorWorker.schedule_periodically(_collectorThread.now() + std::chrono::seconds(_measurementUpperBound),
                                         std::chrono::seconds(API_SAMPLE_INTERVAL_SECONDS), [this](auto) {
                                           auto task = boost::async(boost::launch::async,
                                                                    [this]() { calcConfirmationRateAPICall(); });
                                           _tasks.emplace_back(std::move(task));
                                         });
}

void CRCollector::artificialyDelay() {
  static uint16_t step = 0;
  if (_addtionalLatencyStepSeconds.count()) {
    std::this_thread::sleep_for(
        std::chrono::seconds((step++ % _addtionalLatencyNumSteps) * _addtionalLatencyStepSeconds));
  }
}
void CRCollector::calcConfirmationRateAPICall() {
  auto niOptional = _api->getNodeInfo();
  if (!niOptional.has_value()) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return;
  }

  auto ni = niOptional.value();
  auto tips = std::vector<std::string>{ni.latestMilestone};
  std::vector<std::string> transactions;

  auto now = std::chrono::system_clock::now();
  auto startTimepoint = now - std::chrono::seconds(_measurementUpperBound);
  auto endTimepoint = now - std::chrono::seconds(_measurementLowerBound);

  // We must copy here because the only way to unlock is via DTOR (until we move
  // from libcukoo)
  auto copiedTable = _hashToBroadcastTime;
  auto lt = copiedTable.lock_table();
  auto it = lt.begin();

  while (it != lt.end()) {
    if (it->second.tp < startTimepoint && it->second.tp > endTimepoint) {
      transactions.push_back(it->first);
    }
    ++it;
  }

  if (transactions.empty()) {
    return;
  }

  auto resp = _api->getInclusionStates(transactions, tips);
  if (!resp.has_value() || resp.value().states.size() != transactions.size()) {
    return;
  }
  std::set<std::string> confirmedTransactions;
  uint32_t idx = 0;
  std::copy_if(transactions.begin(), transactions.end(),
               std::inserter(confirmedTransactions, confirmedTransactions.begin()),
               [&](const std::string&) { return resp.value().states[idx++]; });

  calcAndExposeImpl(confirmedTransactions, CONFIRMATION_RATE_API);
}

using namespace prometheus;

void CRCollector::subscribeToTransactions(std::string zmqURL,
                                          const BroadcastReceiveCollector::ZmqObservable& zmqObservable,
                                          std::shared_ptr<Registry> registry) {
  _gauges = buildGaugeMap(registry, "confirmationratecollector", {{"listen_node", _iriHost}, {"zmq_url", zmqURL}},
                          nameToDescGauges);

  zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .subscribe(
          [&](std::shared_ptr<iri::IRIMessage> msg) {
            if (msg->type() != iri::IRIMessageType::SN) return;
            auto tx = std::static_pointer_cast<iri::SNMessage>(std::move(msg));
            _confirmedTransactions.insert(tx->hash());
            calcAndExposeImpl(_confirmedTransactions, CONFIRMATION_RATE_ZMQ);
          },
          []() {});
}
void CRCollector::calcAndExposeImpl(const std::set<std::string>& confirmedTransactions, const std::string label) {
  auto now = std::chrono::system_clock::now();
  auto ub = now - std::chrono::seconds(_measurementUpperBound);
  auto lb = now - std::chrono::seconds(_measurementLowerBound);
  // Map between POW duration (multiple of DELAY_STEP_SECONDS) to the
  // confirmed txs count
  std::map<uint32_t, double> durationToConfirmed;
  std::map<uint32_t, double> durationToTotal;
  double confirmedCount = 0;
  double totalTransactionsCount = 0;

  // Filter transactions that are too old
  auto lt = _hashToBroadcastTime.lock_table();
  auto it = lt.begin();
  while (it != lt.end()) {
    if (it->second.tp < ub && it->second.tp > lb) {
      totalTransactionsCount += 1;
      durationToTotal[it->second.msDuration / (_addtionalLatencyStepSeconds.count() * 1000)] += 1;
      if (confirmedTransactions.find(it->first) != confirmedTransactions.end()) {
        confirmedCount += 1;
        durationToConfirmed[it->second.msDuration / (_addtionalLatencyStepSeconds.count() * 1000)] += 1;
      }
    }
    it++;
  }

  it = lt.begin();
  while (it != lt.end()) {
    if (it->second.tp < lb) {
      it = lt.erase(it);
    } else {
      it++;
    }
  }

  for (auto kv : durationToTotal) {
    double cr = 0;
    if (durationToConfirmed.find(kv.first) != durationToConfirmed.end()) {
      cr = durationToConfirmed[kv.first] / kv.second;
    }
    if (totalTransactionsCount > 0) {
      _gauges.at(label)
          .get()
          .Add({{"pow_duration_group_seconds", std::to_string(kv.first * _addtionalLatencyStepSeconds.count())}})
          .Set(cr);
    }
  }
}
CRCollector::~CRCollector() {
  _collectorWorker.unsubscribe();
  std::for_each(_tasks.begin(), _tasks.end(), [&](auto& task) { task.wait(); });
}
