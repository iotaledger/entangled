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

#include "tanglescope/broadcastrecievecollecter.hpp"
#include "tanglescope/common/iri.hpp"

#include "cppclient/beast.h"

namespace iota {
namespace tanglescope {

class EchoCollector : public BroadcastReceiveCollector {
 public:
  constexpr static auto DISCOVERY_INTERVAL = "discovery_interval";

  constexpr static double BUCKET_WIDTH = 10;
  bool parseConfiguration(const YAML::Node& conf) override;

 protected:  // gmock classes
  boost::future<void> handleUnseenTransactions(std::shared_ptr<iri::TXMessage> tx,
                                               std::chrono::time_point<std::chrono::system_clock> received,
                                               HistogramsMap& histograms, const std::vector<double>& buckets);

 private:
  virtual void doPeriodically() override{};
  const std::vector<double>& histogramBuckets() const;

  virtual void subscribeToTransactions(std::string zmqURL, const ZmqObservable& zmqObservable,
                                       std::shared_ptr<prometheus::Registry> registry) override;
  // Configuration
  uint32_t _discoveryInterval;
  // Others
  cuckoohash_map<std::string, std::chrono::system_clock::time_point> _hashToDiscoveryTime;

  mutable std::shared_mutex _milestoneMutex;
  std::string _latestSolidMilestoneHash;

  static std::map<std::string, std::string> nameToDescHistogram;
};

}  // namespace tanglescope
}  // namespace iota
