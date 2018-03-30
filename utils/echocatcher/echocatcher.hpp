#pragma once

#include <string>
#include <list>
#include <memory>
#include <chrono>
#include <rx.hpp>
#include <prometheus/exposer.h>
#include <libcuckoo/cuckoohash_map.hh>
#include <iota/utils/common/api.hpp>
#include <iota/utils/common/iri.hpp>
#include <iota/utils/prometheus_collector/prometheus_collector.hpp>

namespace iota {
namespace utils {

class EchoCatcher : public PrometheusCollector {
 public:
  // typedef
  typedef std::map<std::string, std::reference_wrapper<
                                    prometheus::Family<prometheus::Gauge>>>
      GaugeMap;
  typedef rxcpp::observable<std::shared_ptr<iri::IRIMessage>> ZmqObservable;
  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;

  struct HashedTX {
    std::string hash;
    std::string tx;
  };

 protected:  // gmock classes
  virtual void loadDB();
  virtual void broadcastTransactions();
  virtual void broadcastOneTransaction();
  virtual void handleReceivedTransactions();
  void handleUnseenTransactions(
      std::shared_ptr<iri::TXMessage> tx,
      cuckoohash_map<std::string, std::chrono::system_clock::time_point>&
          hashToSeenTimestamp,
      std::chrono::time_point<std::chrono::system_clock> received,
      std::weak_ptr<api::IRIClient> iriClient, prometheus::Gauge& timeUntilPublishedGauge);

 private:
  // methods
  GaugeMap buildMetricsMap(
      std::shared_ptr<prometheus::Registry> registry,
      const std::map<std::string, std::string>& labels) override;

  virtual void subscribeToTransactions(
      std::string zmqURL, ZmqObservable& zmqObservable,
      std::shared_ptr<prometheus::Registry> registry);
  // Configuration
  std::string _iriHost;
  std::list<std::string> _zmqPublishers;
  uint32_t _tangleDBWarmupPeriod;
  uint32_t _mwm;
  uint32_t _broadcastInterval;
  // Others
  std::shared_ptr<iota::utils::api::IRIClient> _iriClient;
  std::map<std::string, ZmqObservable> _urlToZmqObservables;
  cuckoohash_map<std::string, std::chrono::system_clock::time_point>
      _hashToBroadcastTime;
  cuckoohash_map<std::string, std::chrono::system_clock::time_point>
      _hashToDiscoveryTime;
};

}  // namespace utils
}  // namespace iota
