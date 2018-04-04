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
  ZMQCollectorImpl(std::string zmqURL, uint32_t pubDelay, uint32_t pubInterval,
                   PrometheusCollector::GaugeMap gaugeFamilies);
  void collect();

 private:
  std::string _zmqURL;
  uint32_t _pubDelay;
  uint32_t _pubInterval;
  PrometheusCollector::GaugeMap _gaugeFamilies;
};
class StatsCollector : public PrometheusCollector {
 public:
  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;

 private:
  virtual PrometheusCollector::GaugeMap buildMetricsMap(
      std::shared_ptr<prometheus::Registry> registry,
      const std::map<std::string, std::string>& labels) override;

  std::unordered_set<std::string> _zmqPublishers;
  uint32_t _pubDelay;
  uint32_t _pubInterval;

  std::list<ZMQCollectorImpl> _zmqCollectors;
};
}  // namespace statscollector

}  // namespace utils
}  // namespace iota
