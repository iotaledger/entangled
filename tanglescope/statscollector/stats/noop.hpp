#pragma once

#include "iota/tanglescope/common/iri.hpp"
#include "stats.hpp"

using namespace iota::tanglescope;

namespace iota {
namespace tanglescope {
namespace statscollector {

class NoopTXStats : public TXStats {
 public:
  virtual void trackNewTX(iri::TXMessage&,
                          PrometheusCollector::CountersMap& metrics){};
  virtual void trackReattachedTX(PrometheusCollector::CountersMap& metrics){};
  virtual void trackNewBundle(PrometheusCollector::CountersMap& metrics){};
  virtual void trackConfirmedBundle(
      int64_t totalValue, uint64_t size, uint64_t avgBundleDuration,
      PrometheusCollector::CountersMap& counters,
      PrometheusCollector::HistogramsMap& histograms){};
};
}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
