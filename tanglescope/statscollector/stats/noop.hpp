#pragma once

#include "stats.hpp"
#include "tanglescope/common/iri.hpp"

using namespace iota::tanglescope;

namespace iota {
namespace tanglescope {
namespace statscollector {

class NoopTXStats : public TXStats {
 public:
  /**
   * Track a new tx
   *
   * @param msg The message
   * @param metric The metric counter map
   */
  virtual void trackNewTX(iri::TXMessage& /* msg */, PrometheusCollector::CountersMap& /* metrics */){};

  /**
   * Track a tx reattachment
   *
   * @param metric The metric counter map
   */
  virtual void trackReattachedTX(PrometheusCollector::CountersMap& /* metrics */){};

  /**
   * Track a new bundle
   *
   * @param metric The metric counter map
   */
  virtual void trackNewBundle(PrometheusCollector::CountersMap& /* metrics */){};

  /**
   * Track a confirmed bundle
   *
   * @param totalValue The total value
   * @param size The size
   * @param avgBundleDuration The bundle confirmation duration
   * @param counters The counters
   * @param histograms The histograms
   */
  virtual void trackConfirmedBundle(int64_t /* totalValue */, uint64_t /* size */, uint64_t /* avgBundleDuration */,
                                    PrometheusCollector::CountersMap& /* counters */,
                                    PrometheusCollector::HistogramsMap& /* histograms */){};
};
}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
