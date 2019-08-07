#pragma once

#include <prometheus/exposer.h>
#include <chrono>
#include <libcuckoo/cuckoohash_map.hh>
#include <list>
#include <rx.hpp>
#include <string>

#include "tanglescope/common/iri.hpp"
#include "tanglescope/prometheus_collector/prometheus_collector.hpp"

namespace iota {
namespace tanglescope {

class BlowballCollector : public PrometheusCollector {
 public:
  constexpr static auto PUBLISHER = "publisher";
  constexpr static auto SNAPSHOT_INTERVAL = "snapshot_interval";
  constexpr static auto HISTOGRAM_RANGE = "histogram_range";
  constexpr static auto BUCKET_SIZE = "bucket_size";

  constexpr static auto TX_NUM_APPROVERS = "tx_num_approvers";
  constexpr static uint8_t EXPIARY_PERIOD = 240; /*seconds*/
  constexpr static uint8_t MAX_NUM_MILESTONES = 30;

  using ZmqObservable = rxcpp::observable<std::shared_ptr<iri::IRIMessage>>;
  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;

  void refCountPublishedTransactions();
  void analyzeBlowballsPeriodically();

 private:
  // methods
  void analyzeBlowballs(const std::vector<double>& buckets);

  // Configuration
  std::string _zmqPublisher;
  uint32_t _snapshotInterval;
  // Others
  PrometheusCollector::HistogramsMap histograms;
  // state
  cuckoohash_map<std::string, uint8_t> _txToRefCount;
  cuckoohash_map<std::string, std::chrono::system_clock::time_point> _txToLastUpdateTime;
  std::list<std::string> _milestones;

  uint32_t _histogramRange;
  uint32_t _bucketSize;

  static std::map<std::string, std::string> nameToDescHistogram;
};

}  // namespace tanglescope
}  // namespace iota
