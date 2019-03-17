#pragma once

#include <cstdint>
#include <memory>
#include <shared_mutex>

#include "tanglescope/common/iri.hpp"

#include "stats.hpp"

using namespace iota::tanglescope;

namespace iota {
namespace tanglescope {
namespace statscollector {

class FrameTXStats : public TXStats {
 public:
  FrameTXStats(uint32_t bundleConfirmationHistogramRange, uint32_t bundleConfirmationHistogramBucketSize);
  void trackNewTX(iri::TXMessage&, PrometheusCollector::CountersMap& counters);
  void trackReattachedTX(PrometheusCollector::CountersMap& counters);
  void trackNewBundle(PrometheusCollector::CountersMap& counters);
  void trackConfirmedBundle(int64_t totalValue, uint64_t size, uint64_t bundleDuration,
                            PrometheusCollector::CountersMap& counters, PrometheusCollector::HistogramsMap& histograms);

  std::vector<double> _buckets;
};
}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
