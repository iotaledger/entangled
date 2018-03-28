#include <iota/utils/prometheus_collector/prometheus_collector.hpp>

namespace iota {
namespace utils {
bool PrometheusCollector::parseConfiguration(const YAML::Node &conf) {
  if (!CollectorBase::parseConfiguration(conf)) {
    return false;
  }
  if (conf["prometheus_exposer_uri"]) {
    _prometheusExpURI = conf["prometheus_exposer_uri"].as<std::string>();
    return true;
  }
  return false;
}
}  // namespace utils
}  // namespace iota