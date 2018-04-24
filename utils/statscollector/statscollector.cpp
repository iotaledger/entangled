#include <atomic>
#include <chrono>
#include <iostream>
#include <map>

#include <cpprest/http_client.h>
#include <glog/logging.h>
#include <rx.hpp>

#include "iota/utils/common/iri.hpp"
#include "iota/utils/common/zmqpub.hpp"

#include "iota/utils/statscollector.hpp"
#include "iota/utils/statscollector/analyzer.hpp"
#include "iota/utils/statscollector/stats/frame.hpp"

using namespace iota::utils;

namespace iota {
namespace utils {
namespace statscollector {

bool StatsCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (!conf[PUBLISHERS]) {
    return false;
  };

  auto listPublishers = conf[PUBLISHERS].as<std::list<std::string>>();
  _zmqPublishers.insert(listPublishers.begin(), listPublishers.end());
  return true;
}

void StatsCollector::collect() {
  using namespace prometheus;

  VLOG(3) << __FUNCTION__;

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();

  for (auto zmqURL : _zmqPublishers) {
    auto counters = buildCountersMap(registry, {{"publish_node", zmqURL}});
    auto histograms = buildHistogramsMap(registry, {{"publish_node", zmqURL}});
    auto collector =
        ZMQCollectorImpl(zmqURL, std::move(counters),
                         std::move(histograms));
    _zmqCollectors.push_back(std::move(collector));
  }
  exposer.RegisterCollectable(registry);

  std::vector<pplx::task<void>> collectorsTasks;
  for (auto& zmqCollector : _zmqCollectors) {
    auto task = pplx::task<void>([&zmqCollector]() { zmqCollector.collect(); });
    collectorsTasks.push_back(std::move(task));
  }

  std::for_each(collectorsTasks.begin(), collectorsTasks.end(),
                [&](pplx::task<void> task) { task.get(); });
}

ZMQCollectorImpl::ZMQCollectorImpl(
    std::string zmqURL, PrometheusCollector::CountersMap counters,
    PrometheusCollector::HistogramsMap histograms)
    : _zmqURL(zmqURL), _counters(counters), _histograms(histograms) {}

void ZMQCollectorImpl::collect() {
  auto stats = std::make_shared<FrameTXStats>();
  auto analyzer = std::make_shared<TXAnalyzer>(_counters, _histograms, stats);

  auto zmqThread = rxcpp::schedulers::make_new_thread();
  auto zmqObservable =
      rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
          [&](auto s) { zmqPublisher(std::move(s), _zmqURL); });

  zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .as_blocking()
      .subscribe(
          [weakAnalyzer =
               std::weak_ptr(analyzer)](std::shared_ptr<iri::IRIMessage> msg) {
            auto analyzer = weakAnalyzer.lock();
            // FIXME (@th0br0) Proper error handling.
            if (!analyzer) return;

            switch (msg->type()) {
              case iri::IRIMessageType::TX:
                analyzer->newTransaction(
                    std::static_pointer_cast<iri::TXMessage>(std::move(msg)));
                break;
              case iri::IRIMessageType::SN:
                analyzer->transactionConfirmed(
                    std::static_pointer_cast<iri::SNMessage>(std::move(msg)));
                break;
              default:
                break;
            };
          },
          []() {});
}

using namespace prometheus;
PrometheusCollector::CountersMap StatsCollector::buildCountersMap(
    std::shared_ptr<Registry> registry,
    const std::map<std::string, std::string>& labels) {
  static std::map<std::string, std::string> nameToDesc = {
      {"transactions_new", "New TXs count"},
      {"transactions_reattached", "Reattached TXs count"},
      {"transactions_confirmed", "confirmed TXs count"},
      {"bundles_new", "new bundles count"},
      {"bundles_confirmed", "confirmed bundles count"},
      {"value_new", "new tx's accumulated value"},
      {"value_confirmed", "confirmed tx's accumulated value"}};

  std::map<std::string, std::reference_wrapper<Family<Counter>>> families;

  for (const auto& kv : nameToDesc) {
    auto& curr_family = BuildCounter()
                            .Name("statscollector_" + kv.first)
                            .Help(kv.second)
                            .Labels(labels)
                            .Register(*registry);

    families.insert(
        std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return std::move(families);
}

PrometheusCollector::HistogramsMap StatsCollector::buildHistogramsMap(
    std::shared_ptr<Registry> registry,
    const std::map<std::string, std::string>& labels) {
  static std::map<std::string, std::string> nameToDesc = {
      {"bundle_confirmation_duration", "bundle's confirmation duration [ms]"}};

  std::map<std::string, std::reference_wrapper<Family<Histogram>>> families;

  for (const auto& kv : nameToDesc) {
    auto& curr_family = BuildHistogram()
                            .Name("statscollector_" + kv.first)
                            .Help(kv.second)
                            .Labels(labels)
                            .Register(*registry);

    families.insert(
        std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return std::move(families);
}

}  // namespace statscollector
}  // namespace utils
}  // namespace iota
