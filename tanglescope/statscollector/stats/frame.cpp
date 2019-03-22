#include <chrono>
#include <memory>
#include <mutex>

#include <glog/logging.h>

#include "tanglescope/common/iri.hpp"
#include "tanglescope/statscollector/stats/frame.hpp"

namespace iota {
namespace tanglescope {
namespace statscollector {

FrameTXStats::FrameTXStats(uint32_t bundleConfirmationHistogramRange, uint32_t bundleConfirmationHistogramBucketSize)
    : _buckets(bundleConfirmationHistogramRange / bundleConfirmationHistogramBucketSize) {
  double currInterval = 0;
  std::generate(_buckets.begin(), _buckets.end(), [&currInterval, bundleConfirmationHistogramBucketSize]() {
    currInterval += bundleConfirmationHistogramBucketSize;
    return currInterval;
  });
}

void FrameTXStats::trackNewTX(iri::TXMessage& tx, PrometheusCollector::CountersMap& counters) {
  VLOG(5) << "trackNewTX({" << tx.value() << ", "
          << std::chrono::duration_cast<std::chrono::milliseconds>(tx.arrivalTime().time_since_epoch()).count() << "})";
  counters.at("transactions_new").get().Add({}).Increment();
  if (tx.value() > 0) {
    auto value = static_cast<int64_t>(tx.value());
    counters.at("value_new").get().Add({}).Increment(value);
    counters.at("value_transactions_new").get().Add({}).Increment();
  }
}
void FrameTXStats::trackReattachedTX(PrometheusCollector::CountersMap& counters) {
  VLOG(5) << "trackReattachedTX";
  counters.at("transactions_reattached").get().Add({}).Increment();
}

void FrameTXStats::trackNewBundle(PrometheusCollector::CountersMap& counters) {
  VLOG(5) << "trackNewBundle()";
  counters.at("bundles_new").get().Add({}).Increment();
}
void FrameTXStats::trackConfirmedBundle(int64_t totalValue, uint64_t size, uint64_t bundleDuration,
                                        PrometheusCollector::CountersMap& counters,
                                        PrometheusCollector::HistogramsMap& histograms) {
  VLOG(5) << "trackConfirmedBundle(" << totalValue << ", " << size << "," << bundleDuration << ")";

  histograms.at("bundle_confirmation_duration").get().Add({}, _buckets).Observe(bundleDuration);

  counters.at("bundles_confirmed").get().Add({}).Increment();

  counters.at("value_confirmed").get().Add({}).Increment(totalValue);

  counters.at("transactions_confirmed").get().Add({}).Increment(size);
  counters.at("value_transactions_confirmed").get().Add({}).Increment(size);
}
}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
