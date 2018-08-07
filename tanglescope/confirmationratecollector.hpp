#pragma once

#include <prometheus/exposer.h>
#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>
#include <chrono>
#include <iota/tanglescope/common/iri.hpp>
#include <libcuckoo/cuckoohash_map.hh>
#include <list>
#include <map>
#include <memory>
#include <rx.hpp>
#include <set>
#include <string>

#include "tanglescope/throwcatchcollector.hpp"

#include "cppclient/beast.h"

namespace iota {
namespace tanglescope {

class CRCollector : public ThrowCatchCollector {
 public:
  constexpr static auto MESAUREMENT_UPPER_BOUND = "measurement_upper_bound";
  constexpr static auto MESAUREMENT_LOWER_BOUND = "measurement_lower_bound";
  constexpr static auto CONFIRMATION_RATE_ZMQ = "confirmation_rate_zmq";
  constexpr static auto CONFIRMATION_RATE_API = "confirmation_rate_api";

  constexpr static uint8_t NUM_VARIANT_DELAYS = 3;
  constexpr static uint8_t MIN_NUM_TRANSACTIONS_TO_REPORT_CR = 2;
  constexpr static uint32_t DELAY_STEP_SECONDS = 5;

  bool parseConfiguration(const YAML::Node& conf) override;

  CRCollector(){};
  ~CRCollector();

 private:
  void subscribeToTransactions(
      std::string zmqURL, const ZmqObservable& zmqObservable,
      std::shared_ptr<prometheus::Registry> registry) override;

  void doPeriodically() override;
  virtual void artificialyDelay() override;

  void calcConfirmationRateAPICall();
  void calcAndExposeImpl(const std::set<std::string>& confirmedTransactions,
                         const std::string label);

  static std::map<std::string, std::string> nameToDescGauges;

  uint32_t _measurementUpperBound;
  uint32_t _measurementLowerBound;

  cuckoohash_map<uint64_t, std::string> _broadcastTimeToHash;
  std::set<std::string> _confirmedTransactions;

  PrometheusCollector::GaugeMap _gauges;
  rxcpp::schedulers::scheduler _collectorThread;
  rxcpp::schedulers::worker _collectorWorker;
  std::vector<boost::future<void>> _tasks;
};

}  // namespace tanglescope
}  // namespace iota
