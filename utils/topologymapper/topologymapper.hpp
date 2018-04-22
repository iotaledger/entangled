#pragma once

#include <prometheus/exposer.h>
#include <chrono>
#include <iota/utils/common/api.hpp>
#include <iota/utils/common/approvedtxdb.hpp>
#include <iota/utils/common/iri.hpp>
#include <iota/utils/prometheus_collector/prometheus_collector.hpp>
#include <list>
#include <memory>
#include <rx.hpp>
#include <shared_mutex>
#include <string>

namespace iota {
namespace utils {

class TopologyMapper : public PrometheusCollector {
 public:
  constexpr static auto PUBLISHERS = "publishers";
  constexpr static auto SNAPSHOT_INTERVAL = "snapshot_interval";

  constexpr static auto TX_NUM_APPROVERS = "tx_num_approvers";
  constexpr static auto NUM_PUBLISHERS = "num_publishers";

  using ZmqObservable = rxcpp::observable<std::shared_ptr<iri::IRIMessage>>;
  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;

  void populateApproversDBs();
  void analyzeBlowballsPeriodically();

 private:
  // methods
  HistogramsMap buildHistogramsMap(
      std::shared_ptr<prometheus::Registry> registry,
      const std::map<std::string, std::string>& labels);

  void analyzeBlowballs();

  virtual void loadApproversDB(std::string zmqURL);

  // Configuration
  std::list<std::string> _zmqPublishers;
  uint32_t _snapshotInterval;
  // Others
  std::map<std::string, ZmqObservable> _urlToZmqObservables;
  std::map<std::string, ApprovedTXDB> _urlToDB;
  PrometheusCollector::HistogramsMap _families;
};

}  // namespace utils
}  // namespace iota
