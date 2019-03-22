#pragma once

#include <string>
#include <unordered_set>

#include <prometheus/exposer.h>

#include "tanglescope/prometheus_collector/prometheus_collector.hpp"

namespace iota {
namespace tanglescope {
namespace statscollector {

constexpr static auto PUBLISHERS = "publishers";
constexpr static auto PUB_INTERVAL = "pub_interval";
constexpr static auto PUB_DELAY = "pub_delay";
constexpr static auto BUNDLE_CONFIRMATION_HISTOGRAM_RANGE = "bundle_confirmation_histogram_range";
constexpr static auto BUNDLE_CONFIRMATION_BUCKET_SIZE = "bundle_confirmation_bucket_size";

constexpr static auto METRIC_PREFIX = "statscollector";

class ZMQCollectorImpl {
 public:
  ZMQCollectorImpl(std::string zmqURL, std::shared_ptr<prometheus::Registry>& registry, bool useURLLable);
  void collect(uint32_t bundleConfirmationHistogramRange, uint32_t bundleConfirmationBucketSize);

 private:
  std::string _zmqURL;
  PrometheusCollector::CountersMap _counters;
  PrometheusCollector::HistogramsMap _histograms;
  PrometheusCollector::GaugeMap _gauges;

  static std::map<std::string, std::string> nameToDescHistograms;
  static std::map<std::string, std::string> nameToDescCounters;
  static std::map<std::string, std::string> nameToDescGauges;
};
class StatsCollector : public PrometheusCollector {
 public:
  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;

 private:
  std::unordered_set<std::string> _zmqPublishers;

  std::list<ZMQCollectorImpl> _zmqCollectors;

  uint32_t _bundleConfirmationHistogramRange;
  uint32_t _bundleConfirmationBucketSize;
};
}  // namespace statscollector

}  // namespace tanglescope
}  // namespace iota
