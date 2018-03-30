#pragma once

#include <string>
#include <prometheus/exposer.h>
#include <iota/utils/prometheus_collector/prometheus_collector.hpp>


namespace iota {
namespace utils {
namespace statscollector {

class StatsCollector : public PrometheusCollector {
 public:
  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;

 private:
  typedef std::map<std::string, std::reference_wrapper<
                                    prometheus::Family<prometheus::Gauge>>>
      GaugeMap;

  virtual GaugeMap buildMetricsMap(
      std::shared_ptr<prometheus::Registry> registry,
      const std::map<std::string, std::string>& labels) override;

  std::string _prometheusExpURI;
  std::string _zmqURL;
  uint32_t _pubDelay;
  uint32_t _pubInterval;
};
}  // namespace statscollector

}  // namespace utils
}  // namespace iota
