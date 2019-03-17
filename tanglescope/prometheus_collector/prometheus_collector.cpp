#include "tanglescope/prometheus_collector/prometheus_collector.hpp"

namespace iota {
namespace tanglescope {
bool PrometheusCollector::parseConfiguration(const YAML::Node& conf) {
  if (!CollectorBase::parseConfiguration(conf)) {
    return false;
  }
  if (conf[PROMETHEUS_EXPOSER_URI]) {
    _prometheusExpURI = conf[PROMETHEUS_EXPOSER_URI].as<std::string>();
    return true;
  }
  return false;
}

using namespace prometheus;

PrometheusCollector::HistogramsMap PrometheusCollector::buildHistogramsMap(
    std::shared_ptr<Registry> registry, const std::string& metricName, const std::map<std::string, std::string>& labels,
    const std::map<std::string, std::string>& nameToDesc) {
  std::map<std::string, std::reference_wrapper<Family<Histogram>>> families;
  for (const auto& kv : nameToDesc) {
    auto& curr_family = BuildHistogram()
                            .Name(metricName + std::string("_") + kv.first)
                            .Help(kv.second)
                            .Labels(labels)
                            .Register(*registry);

    families.insert(std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return families;
}

using namespace prometheus;
PrometheusCollector::CountersMap PrometheusCollector::buildCountersMap(
    std::shared_ptr<Registry> registry, const std::string& metricName, const std::map<std::string, std::string>& labels,
    const std::map<std::string, std::string>& nameToDesc) {
  std::map<std::string, std::reference_wrapper<Family<Counter>>> families;

  for (const auto& kv : nameToDesc) {
    auto& curr_family = BuildCounter()
                            .Name(metricName + std::string("_") + kv.first)
                            .Help(kv.second)
                            .Labels(labels)
                            .Register(*registry);

    families.insert(std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return families;
}

PrometheusCollector::GaugeMap PrometheusCollector::buildGaugeMap(std::shared_ptr<Registry> registry,
                                                                 const std::string& metricName,
                                                                 const std::map<std::string, std::string>& labels,
                                                                 const std::map<std::string, std::string>& nameToDesc) {
  std::map<std::string, std::reference_wrapper<Family<Gauge>>> families;

  for (const auto& kv : nameToDesc) {
    auto& curr_family =
        BuildGauge().Name(metricName + std::string("_") + kv.first).Help(kv.second).Labels(labels).Register(*registry);

    families.insert(std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return families;
}

}  // namespace tanglescope
}  // namespace iota
