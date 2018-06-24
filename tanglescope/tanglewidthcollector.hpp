#pragma once

#include <prometheus/exposer.h>
#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>
#include <chrono>
#include <iota/tanglescope/common/iri.hpp>
#include <iota/tanglescope/prometheus_collector/prometheus_collector.hpp>
#include <list>
#include <rx.hpp>
#include <string>

namespace iota {
namespace tanglescope {

class TangleWidthCollector : public PrometheusCollector {
 public:
  constexpr static auto SNAPSHOT_INTERVAL = "snapshot_interval";
  constexpr static auto MEASURE_LINE_AGE = "measure_line_age";

  constexpr static uint32_t MEASURE_LINE_AGE_STEP = 15;

  constexpr static auto MEASURE_LINE = "measure_line";
  constexpr static auto TANGLE_WIDTH = "tangle_width";

  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;
  virtual ~TangleWidthCollector();

 private:
  void analyzeWidthPeriodically();
  void analyzeWidthImpl();

  // Configuration
  uint32_t _snapshotInterval;
  uint32_t _measureLineAge;

  // Others
  PrometheusCollector::GaugeMap _gauges;

  static std::map<std::string, std::string> nameToDescGauges;
  rxcpp::schedulers::scheduler _collectorThread;
  rxcpp::schedulers::worker _collectorWorker;
  std::vector<boost::future<void>> _tasks;
};

}  // namespace tanglescope
}  // namespace iota
