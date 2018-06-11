#pragma once

#include <prometheus/exposer.h>
#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>
#include <chrono>
#include <iota/tanglescope/common/iri.hpp>
#include <iota/tanglescope/prometheus_collector/prometheus_collector.hpp>
#include <libcuckoo/cuckoohash_map.hh>
#include <list>
#include <memory>
#include <rx.hpp>
#include <shared_mutex>
#include <string>

#include "cppclient/beast.h"

namespace iota {
namespace tanglescope {

class EchoCatcher : public PrometheusCollector {
 public:
  constexpr static auto IRI_HOST = "iri_host";
  constexpr static auto IRI_PORT = "iri_port";
  constexpr static auto PUBLISHERS = "publishers";
  constexpr static auto MWM = "mwm";
  constexpr static auto TANGLE_DB_WARMUP_TIME = "tangledb_warmup_period";
  constexpr static auto BROADCAST_INTERVAL = "broadcast_interval";
  constexpr static auto DISCOVERY_INTERVAL = "discovery_interval";

  constexpr static double BUCKET_WIDTH = 10;
  // typedef
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
  boost::future<void> handleUnseenTransactions(
      std::shared_ptr<iri::TXMessage> tx,
      std::chrono::time_point<std::chrono::system_clock> received,
      HistogramsMap& histograms, const std::vector<double>& buckets);

 private:
  const std::vector<double>& histogramBuckets() const;

  virtual void subscribeToTransactions(
      std::string zmqURL, const ZmqObservable& zmqObservable,
      std::shared_ptr<prometheus::Registry> registry);
  // Configuration
  std::string _iriHost;
  uint32_t _iriPort;
  std::list<std::string> _zmqPublishers;
  uint32_t _tangleDBWarmupPeriod;
  uint32_t _mwm;
  uint32_t _broadcastInterval;
  uint32_t _discoveryInterval;
  // Others
  std::shared_ptr<cppclient::IotaAPI> _api;
  std::map<std::string, ZmqObservable> _urlToZmqObservables;
  cuckoohash_map<std::string, std::chrono::system_clock::time_point>
      _hashToBroadcastTime;
  cuckoohash_map<std::string, std::chrono::system_clock::time_point>
      _hashToDiscoveryTime;

  mutable std::shared_mutex _milestoneMutex;
  std::string _latestSolidMilestoneHash;

  static std::map<std::string, std::string> nameToDescHistogram;
};

}  // namespace tanglescope
}  // namespace iota
