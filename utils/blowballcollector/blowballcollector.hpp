#pragma once

#include <prometheus/exposer.h>
#include <iota/utils/common/iri.hpp>
#include <iota/utils/prometheus_collector/prometheus_collector.hpp>
#include <libcuckoo/cuckoohash_map.hh>
#include <string>

namespace iota {
namespace utils {

class BlowballCollector : public PrometheusCollector {
 public:
  constexpr static auto PUBLISHER = "publisher";
  constexpr static auto SNAPSHOT_INTERVAL = "snapshot_interval";

  constexpr static auto TX_NUM_APPROVERS = "tx_num_approvers";

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
};

}  // namespace utils
}  // namespace iota
