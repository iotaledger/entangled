#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <tuple>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <rx.hpp>
#include <prometheus/exposer.h>

#include "iota/utils/common/iri.hpp"
#include "iota/utils/common/zmqpub.hpp"

#include "iota/utils/statscollector/analyzer.hpp"
#include "iota/utils/statscollector/stats/frame.hpp"


DEFINE_string(zmqURL, "tcp://m5.iotaledger.net:5556",
              "URL of ZMQ publisher to connect to");
DEFINE_uint64(pubInterval, 1000,
              "interval on which to publish to prometheus in milliseconds. You "
              "probably don't want to change this.");
DEFINE_uint64(pubDelay, 120000,
              "interval to wait before starting publishing in milliseconds");

DEFINE_string(prometheusExposerIP, "0.0.0.0:8081",
              "IP/Port that the Prometheus Exposer binds to");

using namespace iota::utils;

namespace iota {
namespace utils {
namespace statscollector {

using namespace prometheus;
std::map<std::string,std::reference_wrapper<Family<Gauge>> > buildMetricsMap(std::shared_ptr<Registry> registry){

    static std::map<std::string,std::string> nameToDesc= {{"transactionsNew","New TXs count"},
                                                    {"transactionsReattached","Reattached TXs count"},
                                                    {"transactionsConfirmed","confirmed TXs count"},
                                                    {"bundlesNew","new bundles count"},
                                                    {"bundlesConfirmed","confirmed bundles count"},
                                                    {"avgConfirmationDuration","bundle's average confirmation duration"},
                                                    {"valueNew","new tx's accumulated value"},
                                                    {"valueConfirmed","confirmed tx's accumulated value"}};

    std::map<std::string,std::reference_wrapper<Family<Gauge>> > famillies;

    for (const auto& kv : nameToDesc) {
    auto&       curr_family = BuildGauge()
                .Name("statscollector_" + kv.first)
                .Help(kv.second)
                .Labels({{"publish_node",FLAGS_zmqURL}})
                .Register(*registry);

    famillies.insert(std::make_pair(std::string(kv.first),std::ref(curr_family)));
    }

    return std::move(famillies);
}

int main(int argc, char** argv) {
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging("statscollector");
  using namespace prometheus;

  LOG(INFO) << "Booting up.";

  auto stats = std::make_shared<FrameTXStats>();

  auto analyzer = std::make_shared<TXAnalyzer>(stats);

  auto zmqThread = rxcpp::schedulers::make_new_thread();
  auto zmqObservable =
      rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
          [&](auto s) { zmqPublisher(std::move(s), FLAGS_zmqURL); })
          .observe_on(rxcpp::observe_on_new_thread());

  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();
  auto pubStartDelay =
      pubThread.now() + std::chrono::milliseconds(FLAGS_pubDelay);
  auto pubInterval = std::chrono::milliseconds(FLAGS_pubInterval);
  
  Exposer exposer{FLAGS_prometheusExposerIP};
  auto registry = std::make_shared<Registry>();

  auto famillies = buildMetricsMap(registry);

  exposer.RegisterCollectable(registry);

  pubWorker.schedule_periodically(pubStartDelay, pubInterval, [
          weakStats = std::weak_ptr(stats), &famillies
  ](auto scbl) {
    if (auto stats = weakStats.lock()) {
      auto frame = stats->swapFrame();
      static bool pubDelayComplete = false;

      if (!pubDelayComplete) {
        pubDelayComplete = true;
      } else {
        {
            famillies.at("transactionsNew").get().Add({}).Set(frame->transactionsNew);
            famillies.at("transactionsReattached").get().Add({}).Set(frame->transactionsReattached);
            famillies.at("transactionsConfirmed").get().Add({}).Set(frame->transactionsConfirmed);
            famillies.at("bundlesNew").get().Add({}).Set(frame->bundlesNew);
            famillies.at("bundlesConfirmed").get().Add({}).Set(frame->bundlesConfirmed);
            famillies.at("avgConfirmationDuration").get().Add({}).Set(frame->avgConfirmationDuration);
            famillies.at("valueNew").get().Add({}).Set(frame->valueNew);
            famillies.at("valueConfirmed").get().Add({}).Set(frame->valueConfirmed);
        }
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

  return 0;
}


}  // namespace statscollector
}  // namespace utils
}  // namespace iota

int main(int argc, char** argv) {
  iota::utils::statscollector::main(argc, argv);
}
