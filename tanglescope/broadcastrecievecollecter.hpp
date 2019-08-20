#pragma once

#include <chrono>
#include <list>
#include <memory>
#include <shared_mutex>
#include <string>

#include <prometheus/exposer.h>
#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>
#include <libcuckoo/cuckoohash_map.hh>
#include <rx.hpp>

#include "cppclient/beast.h"
#include "tanglescope/common/iri.hpp"
#include "tanglescope/prometheus_collector/prometheus_collector.hpp"

namespace iota {
namespace tanglescope {

/*
 * Class Description:
 * This is a base class for collectors that generate transactions
 * (broadcastTransactions) And observe them via zmq (receivedTransactions) so
 * they can collect data. The actual handling of received transactions is done
 * via `subscribeToTransactions` which is pure virtual in this class
 *
 * The Advantage in artificially generating transactions and monitoring them is
 * certainty, with artificial transactions, we know when they were broadcast,
 * and we know if they were approving transactions which are given by doing a
 * tip selection
 */

class BroadcastReceiveCollector : public PrometheusCollector {
 public:
  struct BroadcastInfo {
    std::chrono::system_clock::time_point tp;
    uint64_t msDuration;
  };
  using broadcastInfo = struct BroadcastInfo;
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

  virtual void subscribeToTransactions(std::string zmqURL, const ZmqObservable& zmqObservable,
                                       std::shared_ptr<prometheus::Registry> registry) = 0;

  void receivedTransactions();

  virtual void doPeriodically() = 0;

  // Configuration
  std::string _iriHost;
  uint32_t _iriPort;
  std::list<std::string> _zmqPublishers;
  uint32_t _mwm;
  uint32_t _broadcastInterval;
  // Others
  std::shared_ptr<cppclient::IotaAPI> _api;
  cuckoohash_map<std::string, BroadcastInfo> _hashToBroadcastTime;
};

}  // namespace tanglescope
}  // namespace iota
