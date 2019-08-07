#include <atomic>
#include <chrono>
#include <iostream>
#include <map>

#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>

#include <glog/logging.h>
#include <rx.hpp>

#include "tanglescope/common/iri.hpp"
#include "tanglescope/common/zmqpub.hpp"

#include "tanglescope/common/iri.hpp"
#include "tanglescope/statscollector/analyzer.hpp"
#include "tanglescope/statscollector/stats/frame.hpp"
#include "tanglescope/statscollector/statscollector.hpp"

using namespace iota::tanglescope;

namespace iota {
namespace tanglescope {
namespace statscollector {

std::map<std::string, std::string> ZMQCollectorImpl::nameToDescCounters = {
    {"transactions_new", "New TXs count"},
    {"value_transactions_new", "Value TXs count"},
    {"transactions_reattached", "Reattached TXs count"},
    {"transactions_confirmed", "confirmed TXs count"},
    {"value_transactions_confirmed", "Confirmed value TX's count"},
    {"bundles_new", "new bundles count"},
    {"bundles_confirmed", "confirmed bundles count"},
    {"value_new", "new tx's accumulated value"},
    {"value_confirmed", "confirmed tx's accumulated value"},
    {"milestones_count", "received milestone count"}};

std::map<std::string, std::string> ZMQCollectorImpl::nameToDescHistograms = {
    {"bundle_confirmation_duration", "bundle's confirmation duration [ms]"}};

std::map<std::string, std::string> ZMQCollectorImpl::nameToDescGauges = {
    {"to_process", "Number of transactions to process"},
    {"to_broadcast", "Number of transactions to broadcast to other nodes"},
    {"to_request", "Number of transactions to request from other nodes"},
    {"to_reply", "Number of transactions to reply to nodes who requested"},
    {"total_transactions", "Number of transactions stored in node"}};

bool StatsCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (!conf[PUBLISHERS] || !conf[BUNDLE_CONFIRMATION_HISTOGRAM_RANGE] || !conf[BUNDLE_CONFIRMATION_BUCKET_SIZE]) {
    return false;
  };

  auto listPublishers = conf[PUBLISHERS].as<std::list<std::string>>();
  _zmqPublishers.insert(listPublishers.begin(), listPublishers.end());
  _bundleConfirmationHistogramRange = conf[BUNDLE_CONFIRMATION_HISTOGRAM_RANGE].as<uint32_t>();
  _bundleConfirmationBucketSize = conf[BUNDLE_CONFIRMATION_BUCKET_SIZE].as<uint32_t>();
  return true;
}

void StatsCollector::collect() {
  using namespace prometheus;

  VLOG(3) << __FUNCTION__;

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();

  for (auto zmqURL : _zmqPublishers) {
    auto collector = ZMQCollectorImpl(zmqURL, registry, _zmqPublishers.size() > 1);
    _zmqCollectors.push_back(std::move(collector));
  }
  exposer.RegisterCollectable(registry);

  std::vector<boost::future<void>> collectorsTasks;
  for (auto& zmqCollector : _zmqCollectors) {
    auto fu = boost::async(boost::launch::async, [&zmqCollector, this]() {
      zmqCollector.collect(_bundleConfirmationHistogramRange, _bundleConfirmationBucketSize);
    });
    collectorsTasks.push_back(std::move(fu));
  }

  std::for_each(collectorsTasks.begin(), collectorsTasks.end(), [&](boost::future<void>& fu) { fu.get(); });
}

ZMQCollectorImpl::ZMQCollectorImpl(std::string zmqURL, std::shared_ptr<prometheus::Registry>& registry,
                                   bool useURLLable)
    : _zmqURL(zmqURL) {
  std::map<std::string, std::string> lables;
  if (useURLLable) {
    lables.insert(std::make_pair("publish_node", zmqURL));
  }
  _counters = PrometheusCollector::buildCountersMap(registry, METRIC_PREFIX, lables, nameToDescCounters);
  _histograms = PrometheusCollector::buildHistogramsMap(registry, METRIC_PREFIX, lables, nameToDescHistograms);
  _gauges = PrometheusCollector::buildGaugeMap(registry, METRIC_PREFIX, lables, nameToDescGauges);
}

void ZMQCollectorImpl::collect(uint32_t bundleConfirmationHistogramRange, uint32_t bundleConfirmationBucketSize) {
  auto stats = std::make_shared<FrameTXStats>(bundleConfirmationHistogramRange, bundleConfirmationBucketSize);
  auto analyzer = std::make_shared<TXAnalyzer>(_counters, _histograms, stats);

  if (urlToZmqObservables.find(_zmqURL) == urlToZmqObservables.end()) {
    urlToZmqObservables[_zmqURL] = rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
        [&](auto s) { zmqPublisher(std::move(s), _zmqURL); });
  }

  // Latest solid milestone index
  uint64_t lmsi = 0;

  urlToZmqObservables[_zmqURL]
      .observe_on(rxcpp::synchronize_new_thread())
      .as_blocking()
      .subscribe(
          [weakAnalyzer = std::weak_ptr<TXAnalyzer>(analyzer), &gauges = _gauges, &counters = _counters,
           &lmsi](std::shared_ptr<iri::IRIMessage> msg) {
            auto analyzer = weakAnalyzer.lock();
            // FIXME (@th0br0) Proper error handling.
            if (!analyzer) return;

            switch (msg->type()) {
              case iri::IRIMessageType::TX:
                analyzer->newTransaction(std::static_pointer_cast<iri::TXMessage>(std::move(msg)));
                break;
              case iri::IRIMessageType::SN:
                analyzer->transactionConfirmed(std::static_pointer_cast<iri::SNMessage>(std::move(msg)));
                break;
              case iri::IRIMessageType::LMSI: {
                auto lmsiCurr = std::static_pointer_cast<iri::LMSIMessage>(std::move(msg));
                counters.at("milestones_count").get().Add({}).Increment();
                lmsi = lmsiCurr->latestSolidMilestoneIndex();
              }

              break;
              case iri::IRIMessageType::RSTAT: {
                auto rstatMessage = std::static_pointer_cast<iri::RSTATMessage>(std::move(msg));
                gauges.at("to_process").get().Add({}).Set(rstatMessage->toProcess());
                gauges.at("to_broadcast").get().Add({}).Set(rstatMessage->toBroadcast());
                gauges.at("to_request").get().Add({}).Set(rstatMessage->toRequest());
                gauges.at("to_reply").get().Add({}).Set(rstatMessage->toReply());
                gauges.at("total_transactions").get().Add({}).Set(rstatMessage->totalTransactions());
              } break;
              default:
                break;
            };
          },
          []() {});
}

}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
