#pragma once

#include <prometheus/exposer.h>
#include <iota/utils/prometheus_collector/prometheus_collector.hpp>
#include <string>
#include <unordered_set>

namespace iota {
namespace utils {
namespace statscollector {

constexpr static auto PUBLISHERS = "publishers";
constexpr static auto PUB_INTERVAL = "pub_interval";
constexpr static auto PUB_DELAY = "pub_delay";

class ZMQCollectorImpl {
 public:
  ZMQCollectorImpl(std::string zmqURL,
                   PrometheusCollector::CountersMap counters,
                   PrometheusCollector::HistogramsMap histograms);
  void collect();

 private:
  std::string _zmqURL;
  PrometheusCollector::CountersMap _counters;
  PrometheusCollector::HistogramsMap _histograms;
};
class StatsCollector : public PrometheusCollector {
 public:
  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;

 private:
  virtual PrometheusCollector::CountersMap buildCountersMap(
      std::shared_ptr<prometheus::Registry> registry,
      const std::map<std::string, std::string>& labels);

  virtual PrometheusCollector::HistogramsMap buildHistogramsMap(
      std::shared_ptr<prometheus::Registry> registry,
      const std::map<std::string, std::string>& labels);

  std::unordered_set<std::string> _zmqPublishers;

  std::list<ZMQCollectorImpl> _zmqCollectors;
};
}  // namespace statscollector

}  // namespace utils
}  // namespace iota
