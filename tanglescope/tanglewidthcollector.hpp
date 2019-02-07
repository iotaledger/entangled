#pragma once

#include <chrono>
#include <list>
#include <string>

#include <prometheus/exposer.h>
#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>
#include <rx.hpp>

#include "tanglescope/common/iri.hpp"
#include "tanglescope/prometheus_collector/prometheus_collector.hpp"

namespace iota {
namespace tanglescope {

class TangleWidthCollector : public PrometheusCollector {
 public:
  constexpr static auto SNAPSHOT_INTERVAL = "snapshot_interval";
  constexpr static auto MEASURE_LINE_BASE_AGE = "measure_line_base_age";
  constexpr static auto MEASURE_LINE_MAX_AGE = "measure_line_max_age";
  constexpr static auto MEASURE_LINE_AGE_STEP = "measure_line_age_step";

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
  uint32_t _measureLineBaseAge;
  uint32_t _measureLineMaxAge;
  uint32_t _measureLineAgeStep;

  // Others
  PrometheusCollector::GaugeMap _gauges;

  static std::map<std::string, std::string> nameToDescGauges;
  rxcpp::schedulers::scheduler _collectorThread;
  rxcpp::schedulers::worker _collectorWorker;
  std::vector<boost::future<void>> _tasks;
};

}  // namespace tanglescope
}  // namespace iota
