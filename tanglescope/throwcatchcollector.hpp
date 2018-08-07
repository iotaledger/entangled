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

class ThrowCatchCollector : public PrometheusCollector {
 public:

    struct broadcastInfo {
        std::chrono::system_clock::time_point tp;
        uint64_t msDuration;
    };
    using broadcastInfo = struct broadcastInfo;
  constexpr static auto IRI_HOST = "iri_host";
  constexpr static auto IRI_PORT = "iri_port";
  constexpr static auto PUBLISHERS = "publishers";
  constexpr static auto MWM = "mwm";
  constexpr static auto BROADCAST_INTERVAL = "broadcast_interval";

  using ZmqObservable = rxcpp::observable<std::shared_ptr<iri::IRIMessage>>;
  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;

 protected:  // gmock classes
  virtual void broadcastOneTransaction();
  virtual void artificialyDelay(){};
  virtual void broadcastTransactions();

  virtual void subscribeToTransactions(
      std::string zmqURL, const ZmqObservable& zmqObservable,
      std::shared_ptr<prometheus::Registry> registry) = 0;

  void handleReceivedTransactions();

  virtual void doPeriodically()  = 0;

  // Configuration
  std::string _iriHost;
  uint32_t _iriPort;
  std::list<std::string> _zmqPublishers;
  uint32_t _mwm;
  uint32_t _broadcastInterval;
  // Others
  std::shared_ptr<cppclient::IotaAPI> _api;
  std::map<std::string, ZmqObservable> _urlToZmqObservables;
    cuckoohash_map<std::string, broadcastInfo>
            _hashToBroadcastTime;
};

}  // namespace tanglescope
}  // namespace iota
