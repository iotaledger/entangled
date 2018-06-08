#include <atomic>
#include <chrono>
#include <iostream>
#include <map>

#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>

#include <glog/logging.h>
#include <rx.hpp>

#include "iota/tanglescope/common/iri.hpp"
#include "iota/tanglescope/common/zmqpub.hpp"

#include "iota/tanglescope/statscollector.hpp"
#include "iota/tanglescope/statscollector/analyzer.hpp"
#include "iota/tanglescope/statscollector/stats/frame.hpp"

using namespace iota::tanglescope;

namespace iota {
namespace tanglescope {
namespace statscollector {

bool StatsCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (!conf[PUBLISHERS] || !conf[BUNDLE_CONFIRMATION_HISTOGRAM_RANGE] ||
      !conf[BUNDLE_CONFIRMATION_BUCKET_SIZE]) {
    return false;
  };

  auto listPublishers = conf[PUBLISHERS].as<std::list<std::string>>();
  _zmqPublishers.insert(listPublishers.begin(), listPublishers.end());
  _bundleConfirmationHistogramRange =
      conf[BUNDLE_CONFIRMATION_HISTOGRAM_RANGE].as<uint32_t>();
  _bundleConfirmationBucketSize =
      conf[BUNDLE_CONFIRMATION_BUCKET_SIZE].as<uint32_t>();
  return true;
}

void StatsCollector::collect() {
  using namespace prometheus;

  VLOG(3) << __FUNCTION__;

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();

  for (auto zmqURL : _zmqPublishers) {
    auto collector =
        ZMQCollectorImpl(zmqURL, registry, _zmqPublishers.size() > 1);
    _zmqCollectors.push_back(std::move(collector));
  }
  exposer.RegisterCollectable(registry);

  std::vector<boost::future<void>> collectorsTasks;
  for (auto& zmqCollector : _zmqCollectors) {
    auto fu = boost::async(boost::launch::async, [&zmqCollector, this]() {
      zmqCollector.collect(_bundleConfirmationHistogramRange,
                           _bundleConfirmationBucketSize);
    });
    collectorsTasks.push_back(std::move(fu));
  }

  std::for_each(collectorsTasks.begin(), collectorsTasks.end(),
                [&](boost::future<void>& fu) { fu.get(); });
}

ZMQCollectorImpl::ZMQCollectorImpl(
    std::string zmqURL, std::shared_ptr<prometheus::Registry>& registry,
    bool useURLLable)
    : _zmqURL(zmqURL) {
  std::map<std::string, std::string> lables;
  if (useURLLable) {
    lables.insert(std::make_pair("publish_node", zmqURL));
  }
  _counters = buildCountersMap(registry, lables);
  _histograms = buildHistogramsMap(registry, lables);
  _gauges = buildGaugeMap(registry, lables);
}

void ZMQCollectorImpl::collect(uint32_t bundleConfirmationHistogramRange,
                               uint32_t bundleConfirmationBucketSize) {
  auto stats = std::make_shared<FrameTXStats>(bundleConfirmationHistogramRange,
                                              bundleConfirmationBucketSize);
  auto analyzer = std::make_shared<TXAnalyzer>(_counters, _histograms, stats);

  auto zmqThread = rxcpp::schedulers::make_new_thread();
  auto zmqObservable =
      rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
          [&](auto s) { zmqPublisher(std::move(s), _zmqURL); });

  zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .as_blocking()
      .subscribe(
          [weakAnalyzer = std::weak_ptr(analyzer),
           &gauges = _gauges](std::shared_ptr<iri::IRIMessage> msg) {
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
              case iri::IRIMessageType::RSTAT: {
                auto rstatMessage =
                    std::static_pointer_cast<iri::RSTATMessage>(std::move(msg));
                gauges.at("to_process")
                    .get()
                    .Add({})
                    .Set(rstatMessage->toProcess());
                gauges.at("to_broadcast")
                    .get()
                    .Add({})
                    .Set(rstatMessage->toBroadcast());
                gauges.at("to_request")
                    .get()
                    .Add({})
                    .Set(rstatMessage->toRequest());
                gauges.at("to_reply")
                    .get()
                    .Add({})
                    .Set(rstatMessage->toReply());
                gauges.at("total_transactions")
                    .get()
                    .Add({})
                    .Set(rstatMessage->totalTransactions());
              } break;
              default:
                break;
            };
          },
          []() {});
}

using namespace prometheus;
PrometheusCollector::CountersMap ZMQCollectorImpl::buildCountersMap(
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

PrometheusCollector::HistogramsMap ZMQCollectorImpl::buildHistogramsMap(
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

PrometheusCollector::GaugeMap ZMQCollectorImpl::buildGaugeMap(
    std::shared_ptr<Registry> registry,
    const std::map<std::string, std::string>& labels) {
  static std::map<std::string, std::string> nameToDesc = {
      {"to_process", "Number of transactions to process"},
      {"to_broadcast", "Number of transactions to broadcast to other nodes"},
      {"to_request", "Number of transactions to request from other nodes"},
      {"to_reply", "Number of transactions to reply to nodes who requested"},
      {"total_transactions", "Number of transactions stored in node"}};

  std::map<std::string, std::reference_wrapper<Family<Gauge>>> families;

  for (const auto& kv : nameToDesc) {
    auto& curr_family = BuildGauge()
                            .Name("statscollector_rstat_" + kv.first)
                            .Help(kv.second)
                            .Labels(labels)
                            .Register(*registry);

    families.insert(
        std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return std::move(families);
}

}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
