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

  if (conf[PUBLISHERS] && conf[PUB_INTERVAL] && conf[PUB_DELAY]) {
    auto listPublishers = conf[PUBLISHERS].as<std::list<std::string>>();
    _zmqPublishers.insert(listPublishers.begin(), listPublishers.end());
    _pubInterval = conf[PUB_INTERVAL].as<uint32_t>();
    _pubDelay = conf[PUB_DELAY].as<uint32_t>();
    return true;
  };

  return false;
}

void StatsCollector::collect() {
  using namespace prometheus;

  LOG(INFO) << __FUNCTION__;

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();

  for (auto zmqURL : _zmqPublishers) {
    auto families = buildMetricsMap(registry, {{"publish_node", zmqURL}});
    auto collector =
        ZMQCollectorImpl(zmqURL, _pubDelay, _pubInterval, std::move(families));
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
using namespace prometheus;
PrometheusCollector::GaugeMap StatsCollector::buildMetricsMap(
    std::shared_ptr<Registry> registry,
    const std::map<std::string, std::string>& labels) {
  static std::map<std::string, std::string> nameToDesc = {
      {"transactions_new", "New TXs count"},
      {"transactions_reattached", "Reattached TXs count"},
      {"transactions_confirmed", "confirmed TXs count"},
      {"bundles_new", "new bundles count"},
      {"bundles_confirmed", "confirmed bundles count"},
      {"avg_confirmationDuration", "bundle's average confirmation duration"},
      {"value_new", "new tx's accumulated value"},
      {"value_confirmed", "confirmed tx's accumulated value"}};

  std::map<std::string, std::reference_wrapper<Family<Gauge>>> famillies;

  for (const auto& kv : nameToDesc) {
    auto& curr_family = BuildGauge()
                            .Name("statscollector_" + kv.first)
                            .Help(kv.second)
                            .Labels(labels)
                            .Register(*registry);

    famillies.insert(
        std::make_pair(std::string(kv.first), std::ref(curr_family)));
  }

  return std::move(famillies);
}
ZMQCollectorImpl::ZMQCollectorImpl(std::string zmqURL, uint32_t pubDelay,
                                   uint32_t pubInterval,
                                   PrometheusCollector::GaugeMap gaugeFamilies)
    : _zmqURL(zmqURL),
      _pubDelay(pubDelay),
      _pubInterval(pubInterval),
      _gaugeFamilies(gaugeFamilies) {}

void ZMQCollectorImpl::collect() {
  auto stats = std::make_shared<FrameTXStats>();
  auto analyzer = std::make_shared<TXAnalyzer>(stats);

  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();
  auto pubStartDelay = pubThread.now() + std::chrono::milliseconds(_pubDelay);
  auto pubInterval = std::chrono::milliseconds(_pubInterval);

  auto& thisRef = *this;

  pubWorker.schedule_periodically(
      pubStartDelay, pubInterval,
      [ weakStats = std::weak_ptr(stats), &thisRef ](auto scbl) {
        if (auto stats = weakStats.lock()) {
          auto frame = stats->swapFrame();
          static bool pubDelayComplete = false;

          if (!pubDelayComplete) {
            pubDelayComplete = true;
          } else {
            thisRef._gaugeFamilies.at("transactions_new")
                .get()
                .Add({})
                .Set(frame->transactionsNew);
            thisRef._gaugeFamilies.at("transactions_reattached")
                .get()
                .Add({})
                .Set(frame->transactionsReattached);
            thisRef._gaugeFamilies.at("transactions_confirmed")
                .get()
                .Add({})
                .Set(frame->transactionsConfirmed);
            thisRef._gaugeFamilies.at("bundles_new")
                .get()
                .Add({})
                .Set(frame->bundlesNew);
            thisRef._gaugeFamilies.at("bundles_confirmed")
                .get()
                .Add({})
                .Set(frame->bundlesConfirmed);
            thisRef._gaugeFamilies.at("avg_confirmationDuration")
                .get()
                .Add({})
                .Set(frame->avgConfirmationDuration);
            thisRef._gaugeFamilies.at("value_new")
                .get()
                .Add({})
                .Set(frame->valueNew);
            thisRef._gaugeFamilies.at("value_confirmed")
                .get()
                .Add({})
                .Set(frame->valueConfirmed);
          }
        } else {
          scbl.unsubscribe();
        }
      });

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

}  // namespace statscollector
}  // namespace utils
}  // namespace iota
