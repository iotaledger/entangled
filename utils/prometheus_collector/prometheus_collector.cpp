#include <iota/utils/prometheus_collector/prometheus_collector.hpp>

namespace iota {
namespace utils {
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
}  // namespace utils
}  // namespace iota