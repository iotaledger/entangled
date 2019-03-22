#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "tanglescope/common/iri.hpp"
#include "tanglescope/prometheus_collector/prometheus_collector.hpp"

using namespace iota::tanglescope;

namespace iota {
namespace tanglescope {
namespace statscollector {

class TXStats {
 public:
  virtual void trackNewTX(iri::TXMessage&, PrometheusCollector::CountersMap& metrics) = 0;
  virtual void trackReattachedTX(PrometheusCollector::CountersMap& metrics) = 0;
  virtual void trackNewBundle(PrometheusCollector::CountersMap& metrics) = 0;
  virtual void trackConfirmedBundle(int64_t totalValue, uint64_t size, uint64_t bundleDuration,
                                    PrometheusCollector::CountersMap& counters,
                                    PrometheusCollector::HistogramsMap& histograms) = 0;
};
}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
