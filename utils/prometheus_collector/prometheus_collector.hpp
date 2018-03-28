#pragma once

#include <prometheus/exposer.h>
#include <iota/utils/collector/collector.hpp>
#include <string>

namespace iota {
namespace utils {

class PrometheusCollector : public CollectorBase {
 public:
  // typedefs
  typedef std::map<std::string, std::reference_wrapper<
                                    prometheus::Family<prometheus::Gauge>>>
      GaugeMap;
  // methods
  virtual GaugeMap buildMetricsMap(
      std::shared_ptr<prometheus::Registry> registry,
      const std::map<std::string, std::string>& labels) = 0;
  virtual bool parseConfiguration(const YAML::Node& conf) override;

 protected:
  std::string _prometheusExpURI;
};
}  // namespace utils
}  // namespace iota