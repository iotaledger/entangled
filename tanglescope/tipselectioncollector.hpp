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

class TipSelectionCollector : public PrometheusCollector {
 public:
  constexpr static auto IRI_HOST = "iri_host";
  constexpr static auto IRI_PORT = "iri_port";
  constexpr static auto SAMPLE_INTERVAL = "sample_interval";
  constexpr static auto SAMPLE_SIZE = "sample_size";
  constexpr static auto DEPTH = "depth";
  constexpr static auto DURATION_BUCKET_SIZE = "duration_bucket_size";
  constexpr static auto DURATION_HISTOGRAM_RANGE = "duration_histogram_range";

  // metric names
  constexpr static auto NUM_TX_WAS_SELECTED = "num_tx_was_selected";
  constexpr static auto NUM_TRUNK_EQ_BRANCH = "num_trunk_eq_branch";
  constexpr static auto NUM_TX_WAS_NOT_A_TIP = "num_tx_was_not_a_tip";
  constexpr static auto TIP_SELECTION_DURATION = "tip_selection_duration";

  void collect() override;
  bool parseConfiguration(const YAML::Node& conf) override;

  using ZmqObservable = rxcpp::observable<std::shared_ptr<iri::IRIMessage>>;

 protected:  // gmock classes
  virtual void queryTipSelectionPeriodically();
  virtual void queryTipSelection();

 private:
  // Configuration
  std::string _iriHost;
  uint32_t _iriPort;
  uint32_t _sampleInterval;
  uint16_t _sampleSize;
  uint16_t _depth;
  uint16_t _durationBucketSize;
  uint16_t _durationBucketRange;
  // Others
  ZmqObservable _zmqObservable;
  PrometheusCollector::HistogramsMap _histograms;
  std::vector<double> _txSelectionCountBuckets;
  std::vector<double> _durationBuckets;

  static std::map<std::string, std::string> nameToDescHistogram;

  std::shared_ptr<cppclient::IotaAPI> _api;
};

}  // namespace tanglescope
}  // namespace iota
