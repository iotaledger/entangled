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
   * @param[in, out] msg    The message
   * @param[in, out] metric The metric counter map
   */
  virtual void trackNewTX(iri::TXMessage& /* msg */, PrometheusCollector::CountersMap& /* metrics */){};

  /**
   * Track a tx reattachment
   *
   * @param[in, out] metric The metric counter map
   */
  virtual void trackReattachedTX(PrometheusCollector::CountersMap& /* metrics */){};

  /**
   * Track a new bundle
   *
   * @param[in, out] metric The metric counter map
   */
  virtual void trackNewBundle(PrometheusCollector::CountersMap& /* metrics */){};

  /**
   * Track a confirmed bundle
   *
   * @param[in]      totalValue        The total value
   * @param[in]      size              The size
   * @param[in]      avgBundleDuration The bundle confirmation duration
   * @param[in, out] counters          The counters
   * @param[in, out] histograms        The histograms
   */
  virtual void trackConfirmedBundle(int64_t /* totalValue */, uint64_t /* size */, uint64_t /* avgBundleDuration */,
                                    PrometheusCollector::CountersMap& /* counters */,
                                    PrometheusCollector::HistogramsMap& /* histograms */){};
};
}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
