#include <chrono>
#include <memory>
#include <mutex>

#include <glog/logging.h>

#include <iota/utils/common/iri.hpp>
#include <iota/utils/common/stats.hpp>

namespace iota {
namespace utils {
namespace stats {

std::unique_ptr<StatsFrame> StatsCollector::swapFrame() {
  std::lock_guard guard(_mutex);

  auto old = std::move(_frame);
  _frame = std::make_unique<StatsFrame>();

  return old;
}

void StatsCollector::onNewTransaction(std::shared_ptr<iri::TXMessage> msg) {
  std::lock_guard guard(_mutex);

  // Check if bundle has been confirmed already.
  if (_confirmedBundles.find(msg->bundle()) != _confirmedBundles.end()) {
    VLOG(7) << "onNewTransaction(bundle: " << msg->bundle()
            << ") already confirmed";
    trackReattachedTX();
    return;
  }

  auto entry = _unconfirmedBundles.find(msg->bundle());
  auto index = msg->currentIndex();

  if (entry == _unconfirmedBundles.end()) {
    // Bundle is new. Set up everything.
    trackNewBundle();
    auto vec =
        std::vector<std::unique_ptr<UnconfirmedTX>>(msg->lastIndex() + 1);
    auto unconfirmed = std::make_unique<UnconfirmedTX>(msg);
    trackNewTX(*unconfirmed);
    vec.at(index) = std::move(unconfirmed);

    _unconfirmedBundles.insert(
        std::make_pair<>(std::string(msg->bundle()), std::move(vec)));
  } else {
    // Bundle was seen before.
    // Was this bundle tx seen before?
    if (entry->second.at(index) == nullptr) {
      // No, so add & track.
      auto unconfirmed = std::make_unique<UnconfirmedTX>(msg);
      trackNewTX(*unconfirmed);
      entry->second.at(index) = std::move(unconfirmed);
    } else {
      // Yes, so track reattachment.
      trackReattachedTX();
    }
  }
}

void StatsCollector::onTransactionConfirmed(
    std::shared_ptr<iri::SNMessage> msg) {
  std::lock_guard guard(_mutex);

  const auto entry = _unconfirmedBundles.find(msg->bundle());

  if (_confirmedBundles.find(msg->bundle()) != _confirmedBundles.end() ||
      !_unconfirmedBundles.count(msg->bundle())) {
    // Confirmed already or we haven't seen this bundle before and thus are
    // ignoring it on purpose.
    VLOG(7) << "onTransactionConfirmed(bundle: " << msg->bundle()
            << "): discarding.";
    return;
  }

  uint64_t size = 0;
  int64_t totalValue = 0;
  uint64_t duration = 0;

  VLOG(7) << "onTransactionConfirmed(bundle: " << msg->bundle() << ")";

  for (const std::unique_ptr<UnconfirmedTX>& tx : entry->second) {
    if (tx) {
      // Maybe we've only seen part of the bundle.
      if (tx->value > 0) {
        totalValue += tx->value;
      }

      auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now() - tx->arrivalTime)
                           .count();

      // We normalise on bundle here
      duration = (elapsedMs + size * duration) / (size + 1);
    }
    size++;
  }

  trackConfirmedBundle(totalValue, size, duration);

  _unconfirmedBundles.erase(entry);
}

void StatsCollector::trackNewTX(UnconfirmedTX& tx) {
  VLOG(5) << "trackNewTX({" << tx.value << ", "
          << std::chrono::duration_cast<std::chrono::milliseconds>(
                 tx.arrivalTime.time_since_epoch())
                 .count()
          << "})";
  _frame->transactionsNew++;
  if (tx.value > 0) {
    _frame->valueNew += static_cast<int64_t>(tx.value);
  }
}
void StatsCollector::trackReattachedTX() {
  VLOG(5) << "trackReattachedTX";
  _frame->transactionsReattached++;
}

void StatsCollector::trackNewBundle() {
  VLOG(5) << "trackNewBundle()";
  _frame->bundlesNew++;
}
void StatsCollector::trackConfirmedBundle(int64_t totalValue, uint64_t size,
                                          uint64_t avgBundleDuration) {
  VLOG(5) << "trackConfirmedBundle(" << totalValue << ", " << size << ","
          << avgBundleDuration << ")";

  _frame->avgConfirmationDuration =
      (avgBundleDuration +
       _frame->bundlesConfirmed * _frame->avgConfirmationDuration) /
      (_frame->bundlesConfirmed + 1);
  _frame->bundlesConfirmed++;

  _frame->valueConfirmed += totalValue;
  _frame->transactionsConfirmed += size;
}

}  // namespace stats
}  // namespace utils
}  // namespace iota
