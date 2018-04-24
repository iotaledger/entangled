#pragma once

#include <cstdint>
#include <memory>
#include <shared_mutex>

#include "iota/utils/common/iri.hpp"

#include "stats.hpp"

using namespace iota::utils;

namespace iota {
namespace utils {
namespace statscollector {

class FrameTXStats : public TXStats {
 public:
  FrameTXStats();
  void trackNewTX(iri::TXMessage&, PrometheusCollector::CountersMap& counters);
  void trackReattachedTX(PrometheusCollector::CountersMap& counters);
  void trackNewBundle(PrometheusCollector::CountersMap& counters);
  void trackConfirmedBundle(int64_t totalValue, uint64_t size,
                            uint64_t bundleDuration,
                            PrometheusCollector::CountersMap& counters,
                            PrometheusCollector::HistogramsMap& histograms);

  std::vector<double> _buckets;
};
}  // namespace statscollector
}  // namespace utils
}  // namespace iota
