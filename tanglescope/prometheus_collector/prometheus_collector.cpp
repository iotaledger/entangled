#include <iota/tanglescope/prometheus_collector/prometheus_collector.hpp>

namespace iota {
namespace tanglescope {
bool PrometheusCollector::parseConfiguration(const YAML::Node &conf) {
  if (!CollectorBase::parseConfiguration(conf)) {
    return false;
  }
  if (conf[PROMETHEUS_EXPOSER_URI]) {
    _prometheusExpURI = conf[PROMETHEUS_EXPOSER_URI].as<std::string>();
    return true;
  }
  return false;
}
}  // namespace tanglescope
}  // namespace iota