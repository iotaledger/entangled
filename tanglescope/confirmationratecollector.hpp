#pragma once

#include <chrono>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>

#include <prometheus/exposer.h>
#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>
#include <libcuckoo/cuckoohash_map.hh>
#include <rx.hpp>

#include "cppclient/beast.h"
#include "tanglescope/broadcastrecievecollecter.hpp"
#include "tanglescope/common/iri.hpp"

namespace iota {
namespace tanglescope {

class CRCollector : public BroadcastReceiveCollector {
 public:
  // Conf
  constexpr static auto MESAUREMENT_UPPER_BOUND = "measurement_upper_bound";
  constexpr static auto MESAUREMENT_LOWER_BOUND = "measurement_lower_bound";
  constexpr static auto CONFIRMATION_RATE_ZMQ = "confirmation_rate_zmq";
  constexpr static auto CONFIRMATION_RATE_API = "confirmation_rate_api";
  constexpr static auto ENABLE_CR_FROM_API = "enable_cr_from_api";
  constexpr static auto ADDITIONAL_LATENCY_STEP_SECONDS = "additional_latency_step_seconds";
  constexpr static auto ADDITIONAL_LATENCY_NUM_STEPS = "additional_latency_num_steps";

  // Defs
  constexpr static uint16_t API_SAMPLE_INTERVAL_SECONDS = 10;

  bool parseConfiguration(const YAML::Node& conf) override;

  CRCollector(){};
  ~CRCollector();

 private:
  void subscribeToTransactions(std::string zmqURL, const ZmqObservable& zmqObservable,
                               std::shared_ptr<prometheus::Registry> registry) override;

  void doPeriodically() override;
  virtual void artificialyDelay() override;

  void calcConfirmationRateAPICall();
  void calcAndExposeImpl(const std::set<std::string>& confirmedTransactions, const std::string label);

  static std::map<std::string, std::string> nameToDescGauges;

  uint32_t _measurementUpperBound;
  uint32_t _measurementLowerBound;

  std::set<std::string> _confirmedTransactions;

  PrometheusCollector::GaugeMap _gauges;
  rxcpp::schedulers::scheduler _collectorThread;
  rxcpp::schedulers::worker _collectorWorker;
  std::vector<boost::future<void>> _tasks;
  bool _enableApi;
  std::chrono::seconds _addtionalLatencyStepSeconds;
  uint32_t _addtionalLatencyNumSteps;
};

}  // namespace tanglescope
}  // namespace iota
