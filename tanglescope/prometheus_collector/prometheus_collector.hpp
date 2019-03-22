#pragma once

#include <prometheus/exposer.h>
#include <string>

#include "tanglescope/collector/collector.hpp"

namespace iota {
namespace tanglescope {

class PrometheusCollector : public CollectorBase {
 public:
  constexpr static auto PROMETHEUS_EXPOSER_URI = "prometheus_exposer_uri";
  // typedefs
  typedef std::map<std::string, std::reference_wrapper<prometheus::Family<prometheus::Counter>>> CountersMap;
  typedef std::map<std::string, std::reference_wrapper<prometheus::Family<prometheus::Gauge>>> GaugeMap;

  typedef std::map<std::string, std::reference_wrapper<prometheus::Family<prometheus::Histogram>>> HistogramsMap;
  // methods
  bool parseConfiguration(const YAML::Node& conf) override;

  static HistogramsMap buildHistogramsMap(std::shared_ptr<prometheus::Registry> registry, const std::string& metricName,
                                          const std::map<std::string, std::string>& labels,
                                          const std::map<std::string, std::string>& nameToDesc);

  static CountersMap buildCountersMap(std::shared_ptr<prometheus::Registry> registry, const std::string& metricName,
                                      const std::map<std::string, std::string>& labels,
                                      const std::map<std::string, std::string>& nameToDesc);

  static GaugeMap buildGaugeMap(std::shared_ptr<prometheus::Registry> registry, const std::string& metricName,
                                const std::map<std::string, std::string>& labels,
                                const std::map<std::string, std::string>& nameToDesc);

 protected:
  std::string _prometheusExpURI;
};
}  // namespace tanglescope
}  // namespace iota
