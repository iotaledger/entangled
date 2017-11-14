#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <influxdb/influxdb_line.h>
#include <influxdb/influxdb_simple_async_api.h>
#include <rx.hpp>

#include "iota/utils/common/iri.hpp"
#include "iota/utils/common/zmqpub.hpp"

#include "iota/utils/statscollector/analyzer.hpp"
#include "iota/utils/statscollector/stats/frame.hpp"

DEFINE_string(zmqURL, "tcp://m5.iotaledger.net:5556",
              "URL of ZMQ publisher to connect to");
DEFINE_string(influxURL, "http://localhost:8086", "InfluxDB URL to connect to");
DEFINE_string(influxDB, "iristats", "InfluxDB db name");
DEFINE_uint64(pubInterval, 1000,
              "interval on which to publish to InfluxDB in milliseconds. You "
              "probably don't want to change this.");
DEFINE_uint64(pubDelay, 120000,
              "interval to wait before starting publishing in milliseconds");

using namespace iota::utils;

namespace iota {
namespace utils {
namespace statscollector {

int main(int argc, char** argv) {
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging("statscollector");

  LOG(INFO) << "Booting up.";

  auto stats = std::make_shared<FrameTXStats>();
  auto analyzer = std::make_shared<TXAnalyzer>(stats);

  auto zmqThread = rxcpp::schedulers::make_new_thread();
  auto zmqObservable =
      rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
          [&](auto s) { zmqPublisher(std::move(s), FLAGS_zmqURL); })
          .observe_on(rxcpp::observe_on_new_thread());

  influxdb::async_api::simple_db influxdb(FLAGS_influxURL, FLAGS_influxDB);

  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();
  auto pubStartDelay =
      pubThread.now() + std::chrono::milliseconds(FLAGS_pubDelay);
  auto pubInterval = std::chrono::milliseconds(FLAGS_pubInterval);

  pubWorker.schedule_periodically(pubStartDelay, pubInterval, [
    &influxdb, weakStats = std::weak_ptr(stats)
  ](auto scbl) {
    if (auto stats = weakStats.lock()) {
      auto frame = stats->swapFrame();
      static bool pubDelayComplete = false;

      if (!pubDelayComplete) {
        pubDelayComplete = true;
      } else {
        LOG(INFO) << "Logging to InfluxDB.";
        {
          using namespace ::influxdb::api;

          influxdb.insert(line(
              "stats", key_value_pairs(),
              key_value_pairs("transactionsNew", frame->transactionsNew)
                  .add("transactionsReattached", frame->transactionsReattached)
                  .add("transactionsTotal",
                       frame->transactionsNew + frame->transactionsReattached)
                  .add("transactionsConfirmed", frame->transactionsConfirmed)
                  .add("bundlesNew", frame->bundlesNew)
                  .add("bundlesConfirmed", frame->bundlesConfirmed)
                  .add("avgConfirmationDuration",
                       frame->avgConfirmationDuration)
                  .add("valueNew", frame->valueNew)
                  .add("valueConfirmed", frame->valueConfirmed),
              default_timestamp()));
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
