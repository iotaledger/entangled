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

#include <statscollector/iri.hpp>
#include <statscollector/stats.hpp>
#include <statscollector/zmq.hpp>

DEFINE_string(zmqURL, "tcp://m5.iotaledger.net:5556",
              "URL of ZMQ publisher to connect to");
DEFINE_string(influxURL, "http://localhost:8086", "InfluxDB URL to connect to");
DEFINE_string(influxDB, "iristats", "InfluxDB db name");
DEFINE_uint64(pubInterval, 1000,
              "interval on which to publish to InfluxDB in milliseconds. You "
              "probably don't want to change this.");
DEFINE_uint64(pubDelay, 120000,
              "interval to wait before starting publishing in milliseconds");

int main(int argc, char** argv) {
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging("statscollector");

  LOG(INFO) << "Booting up.";

  auto collector = std::make_shared<stats::StatsCollector>();

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
    &influxdb, weakCollector = std::weak_ptr(collector)
  ](auto scbl) {
    static bool pubDelayComplete = false;
    if (auto collector = weakCollector.lock()) {
      auto frame = collector->swapFrame();
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

  zmqObservable.subscribe_on(rxcpp::observe_on_event_loop())
      .as_blocking()
      .subscribe(
          [weakCollector =
               std::weak_ptr(collector)](std::shared_ptr<iri::IRIMessage> msg) {
            auto collector = weakCollector.lock();
            // FIXME (@th0br0) Proper error handling.
            if (!collector) return;

            switch (msg->type()) {
              case iri::IRIMessageType::TX:
                collector->onNewTransaction(
                    std::static_pointer_cast<iri::TXMessage>(std::move(msg)));
                break;
              case iri::IRIMessageType::SN:
                collector->onTransactionConfirmed(
                    std::static_pointer_cast<iri::SNMessage>(std::move(msg)));
                break;
              default:
                break;
            };
          },
          []() {});

  return 0;
}
