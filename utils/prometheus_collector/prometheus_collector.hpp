#pragma once

#include <prometheus/exposer.h>
#include <iota/utils/collector/collector.hpp>
#include <string>

namespace iota {
namespace utils {

class PrometheusCollector : public CollectorBase {
 public:
  constexpr static auto PROMETHEUS_EXPOSER_URI = "prometheus_exposer_uri";
  // typedefs
  typedef std::map<std::string, std::reference_wrapper<
          prometheus::Family<prometheus::Counter>>>
  CountersMap;
  typedef std::map<std::string, std::reference_wrapper<
                                    prometheus::Family<prometheus::Gauge>>>
      GaugeMap;

  typedef std::map<std::string, std::reference_wrapper<
          prometheus::Family<prometheus::Histogram>>>
  HistogramsMap;
  // methods
  virtual bool parseConfiguration(const YAML::Node& conf) override;

 protected:
  std::string _prometheusExpURI;
};
}  // namespace utils
}  // namespace iota