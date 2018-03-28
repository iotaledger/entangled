#include <atomic>
#include <chrono>
#include <iostream>
#include <map>

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

  if (conf["publisher"] && conf["pub_interval"] && conf["pub_delay"]) {
    _zmqURL = conf["publisher"].as<std::string>();
    _pubInterval = conf["pub_interval"].as<uint32_t>();
    _pubDelay = conf["pub_delay"].as<uint32_t>();
    return true;
  };

  return false;
}

void StatsCollector::collect() {
  using namespace prometheus;

  LOG(INFO) << __FUNCTION__;

  auto stats = std::make_shared<FrameTXStats>();

  auto analyzer = std::make_shared<TXAnalyzer>(stats);

  auto zmqThread = rxcpp::schedulers::make_new_thread();
  auto zmqObservable =
      rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
          [&](auto s) { zmqPublisher(std::move(s), _zmqURL); })
          .observe_on(rxcpp::observe_on_new_thread());

  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();
  auto pubStartDelay = pubThread.now() + std::chrono::milliseconds(_pubDelay);
  auto pubInterval = std::chrono::milliseconds(_pubInterval);

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();

  auto families = buildMetricsMap(registry, {{"publish_node", _zmqURL}});

  exposer.RegisterCollectable(registry);

  pubWorker.schedule_periodically(pubStartDelay, pubInterval, [
    weakStats = std::weak_ptr(stats), &families
  ](auto scbl) {
    if (auto stats = weakStats.lock()) {
      auto frame = stats->swapFrame();
      static bool pubDelayComplete = false;

      if (!pubDelayComplete) {
        pubDelayComplete = true;
      } else {
        families.at("transactions_new")
            .get()
            .Add({})
            .Set(frame->transactionsNew);
        families.at("transactions_reattached")
            .get()
            .Add({})
            .Set(frame->transactionsReattached);
        families.at("transactions_confirmed")
            .get()
            .Add({})
            .Set(frame->transactionsConfirmed);
        families.at("bundles_new").get().Add({}).Set(frame->bundlesNew);
        families.at("bundles_confirmed")
            .get()
            .Add({})
            .Set(frame->bundlesConfirmed);
        families.at("avg_confirmationDuration")
            .get()
            .Add({})
            .Set(frame->avgConfirmationDuration);
        families.at("value_new").get().Add({}).Set(frame->valueNew);
        families.at("value_confirmed").get().Add({}).Set(frame->valueConfirmed);
      }
    } else {
      scbl.unsubscribe();
    }
  });

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
StatsCollector::GaugeMap StatsCollector::buildMetricsMap(
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

}  // namespace statscollector
}  // namespace utils
}  // namespace iota
