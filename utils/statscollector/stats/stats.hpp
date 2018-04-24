#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <chrono>
#include <cstdint>
#include <memory>
#include <shared_mutex>

#include <iota/utils/prometheus_collector/prometheus_collector.hpp>
#include "iota/utils/common/iri.hpp"

using namespace iota::utils;

namespace iota {
namespace utils {
namespace statscollector {

class TXStats {
 public:
  virtual void trackNewTX(iri::TXMessage&,
                          PrometheusCollector::CountersMap& metrics) = 0;
  virtual void trackReattachedTX(PrometheusCollector::CountersMap& metrics) = 0;
  virtual void trackNewBundle(PrometheusCollector::CountersMap& metrics) = 0;
  virtual void trackConfirmedBundle(
      int64_t totalValue, uint64_t size, uint64_t bundleDuration,
      PrometheusCollector::CountersMap& counters,
      PrometheusCollector::HistogramsMap& histograms) = 0;
};
}  // namespace statscollector
}  // namespace utils
}  // namespace iota
