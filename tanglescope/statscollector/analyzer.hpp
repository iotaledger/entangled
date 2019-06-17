#pragma once

#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <chrono>
#include <cstdint>
#include <memory>
#include <shared_mutex>

#include "tanglescope/common/iri.hpp"
#include "tanglescope/prometheus_collector/prometheus_collector.hpp"

#include "stats/noop.hpp"
#include "stats/stats.hpp"

using namespace iota::tanglescope;

namespace iota {
namespace tanglescope {
namespace statscollector {

class TXAnalyzer {
 public:
  explicit TXAnalyzer(PrometheusCollector::CountersMap& counters, PrometheusCollector::HistogramsMap& histograms,
                      std::shared_ptr<TXStats> stats = std::make_shared<NoopTXStats>())
      : _counters(counters), _histograms(histograms), _stats(std::move(stats)) {}

  void newTransaction(std::shared_ptr<iri::TXMessage>);
  void transactionConfirmed(std::shared_ptr<iri::SNMessage>);

 private:
  std::shared_mutex _mutex;

  std::list<std::string> _confirmedBundles;
  std::unordered_map<std::string, std::vector<std::shared_ptr<iri::TXMessage>>> _unconfirmedBundles;

  PrometheusCollector::CountersMap& _counters;
  PrometheusCollector::HistogramsMap& _histograms;
  std::shared_ptr<TXStats> _stats;
};
}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
