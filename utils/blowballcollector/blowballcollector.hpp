#pragma once

#include <prometheus/exposer.h>
#include <chrono>
#include <iota/utils/common/iri.hpp>
#include <iota/utils/prometheus_collector/prometheus_collector.hpp>
#include <libcuckoo/cuckoohash_map.hh>
#include <list>
#include <rx.hpp>
#include <string>

namespace iota {
namespace utils {

class BlowballCollector : public PrometheusCollector {
 public:
  constexpr static auto PUBLISHER = "publisher";
  constexpr static auto SNAPSHOT_INTERVAL = "snapshot_interval";

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
  HistogramsMap buildHistogramsMap(
      std::shared_ptr<prometheus::Registry> registry,
      const std::map<std::string, std::string>& labels);

  void analyzeBlowballs(const std::vector<double>& buckets);

  // Configuration
  std::string _zmqPublisher;
  uint32_t _snapshotInterval;
  // Others
  ZmqObservable _zmqObservable;
  PrometheusCollector::HistogramsMap _families;
  // state
  cuckoohash_map<std::string, uint8_t> _txToRefCount;
  cuckoohash_map<std::string, std::chrono::system_clock::time_point>
      _txToLastUpdateTime;
  std::list<std::string> _milestones;
};

}  // namespace utils
}  // namespace iota
