#pragma once

#include <prometheus/exposer.h>
#include <iota/tanglescope/prometheus_collector/prometheus_collector.hpp>
#include <string>
#include <unordered_set>

namespace iota {
namespace tanglescope {
namespace statscollector {

constexpr static auto PUBLISHERS = "publishers";
constexpr static auto PUB_INTERVAL = "pub_interval";
constexpr static auto PUB_DELAY = "pub_delay";
constexpr static auto BUNDLE_CONFIRMATION_HISTOGRAM_RANGE =
    "bundle_confirmation_histogram_range";
constexpr static auto BUNDLE_CONFIRMATION_BUCKET_SIZE =
    "bundle_confirmation_bucket_size";

class ZMQCollectorImpl {
 public:
  ZMQCollectorImpl(std::string zmqURL,
                   PrometheusCollector::CountersMap counters,
                   PrometheusCollector::HistogramsMap histograms,
                   PrometheusCollector::GaugeMap gauges);
  void collect(uint32_t bundleConfirmationHistogramRange,
               uint32_t bundleConfirmationBucketSize);

 private:
  std::string _zmqURL;
  PrometheusCollector::CountersMap _counters;
  PrometheusCollector::HistogramsMap _histograms;
  PrometheusCollector::GaugeMap _gauges;
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

  virtual PrometheusCollector::GaugeMap buildGaugeMap(
      std::shared_ptr<prometheus::Registry> registry,
      const std::map<std::string, std::string>& labels);

  std::unordered_set<std::string> _zmqPublishers;

  std::list<ZMQCollectorImpl> _zmqCollectors;

  uint32_t _bundleConfirmationHistogramRange;
  uint32_t _bundleConfirmationBucketSize;
};
}  // namespace statscollector

}  // namespace tanglescope
}  // namespace iota
