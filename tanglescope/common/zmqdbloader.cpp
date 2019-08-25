#include "tanglescope/common/zmqdbloader.hpp"

#include <glog/logging.h>

#include "tanglescope/common/tangledb.hpp"
#include "tanglescope/common/zmqpub.hpp"

namespace iota {
namespace tanglescope {

bool ZmqDBLoader::parseConfiguration(const YAML::Node& conf) {
  if (!conf) {
    return false;
  }

  if (!conf[PUBLISHER] || !conf[OLDEST_TX_AGE] || !conf[CLEANUP_INTERVAL]) {
    return false;
  }
  _zmqPublisherURL = conf[PUBLISHER].as<std::string>();
  _oldestTXAge = conf[OLDEST_TX_AGE].as<uint32_t>();
  _cleanupInterval = conf[CLEANUP_INTERVAL].as<uint32_t>();
  return true;
}

void ZmqDBLoader::start() {
  VLOG(3) << __FUNCTION__;

  if (urlToZmqObservables.find(_zmqPublisherURL) == urlToZmqObservables.end()) {
    urlToZmqObservables[_zmqPublisherURL] = rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
        [&](auto s) { zmqPublisher(std::move(s), _zmqPublisherURL); });
  }

  cleanDBPeriodically();
  loadDB();
}

void ZmqDBLoader::cleanDBPeriodically() {
  auto pubThread = rxcpp::schedulers::make_new_thread();
  auto pubWorker = pubThread.create_worker();

  if (_cleanupInterval > 0) {
    pubWorker.schedule_periodically(
        pubThread.now(), std::chrono::seconds(_cleanupInterval),
        [oldestTXAge = _oldestTXAge](auto) { TangleDB::instance().removeAgedTxs(oldestTXAge); });
  }
}

void ZmqDBLoader::loadDB() {
  urlToZmqObservables[_zmqPublisherURL]
      .observe_on(rxcpp::synchronize_new_thread())
      .subscribe(
          [](std::shared_ptr<iri::IRIMessage> msg) {
            if (msg->type() != iri::IRIMessageType::TX) return;

            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));

            TangleDB::TXRecord txRec = {tx->hash(), tx->trunk(), tx->branch(), tx->timestamp()};
            TangleDB::instance().put(std::move(txRec));
          },
          []() {});
}

}  // namespace tanglescope

}  // namespace iota
