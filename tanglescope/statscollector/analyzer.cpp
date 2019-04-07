#include <chrono>
#include <memory>
#include <mutex>

#include <glog/logging.h>

#include "tanglescope/common/iri.hpp"

#include "tanglescope/statscollector/analyzer.hpp"

namespace iota {
namespace tanglescope {
namespace statscollector {

static constexpr uint32_t MAX_CONFIRMED_BUNDLES_TO_KEEP = 10000;

void TXAnalyzer::newTransaction(std::shared_ptr<iri::TXMessage> msg) {
  std::lock_guard guard(_mutex);

  // Check if bundle has been confirmed already.
  if (std::find(_confirmedBundles.begin(), _confirmedBundles.end(), msg->bundle()) != _confirmedBundles.end()) {
    VLOG(7) << "onNewTransaction(bundle: " << msg->bundle() << ") already confirmed";
    _stats->trackReattachedTX(_counters);
    return;
  }

  auto entry = _unconfirmedBundles.find(msg->bundle());
  auto index = msg->currentIndex();

  if (entry == _unconfirmedBundles.end()) {
    // Bundle is new. Set up everything.
    _stats->trackNewBundle(_counters);
    auto vec = std::vector<std::shared_ptr<iri::TXMessage>>(msg->lastIndex() + 1);
    _stats->trackNewTX(*msg, _counters);

    auto bundle = msg->bundle();
    std::string bundleHash = msg->bundle();
    vec.at(index) = std::move(msg);

    _unconfirmedBundles.insert(std::make_pair<>(std::move(bundleHash), std::move(vec)));
  } else {
    // Bundle was seen before.
    // Was this bundle tx seen before?
    if (entry->second.at(index) == nullptr) {
      // No, so add & track.
      _stats->trackNewTX(*msg, _counters);
      entry->second.at(index) = std::move(msg);
    } else {
      // Yes, so track reattachment.
      _stats->trackReattachedTX(_counters);
    }
  }
}

void TXAnalyzer::transactionConfirmed(std::shared_ptr<iri::SNMessage> msg) {
  std::lock_guard guard(_mutex);

  const auto entry = _unconfirmedBundles.find(msg->bundle());

  if (std::find(_confirmedBundles.begin(), _confirmedBundles.end(), msg->bundle()) != _confirmedBundles.end() ||
      !_unconfirmedBundles.count(msg->bundle())) {
    // Confirmed already or we haven't seen this bundle before and thus are
    // ignoring it on purpose.
    VLOG(7) << "onTransactionConfirmed(bundle: " << msg->bundle() << "): discarding.";
    return;
  }

  uint64_t size = 0;
  int64_t totalValue = 0;
  uint64_t duration = 0;

  VLOG(7) << "onTransactionConfirmed(bundle: " << msg->bundle() << ")";

  for (const std::shared_ptr<iri::TXMessage>& tx : entry->second) {
    if (tx) {
      // Maybe we've only seen part of the bundle.
      if (tx->value() > 0) {
        totalValue += tx->value();
      }

      auto elapsedSeconds =
          std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - tx->arrivalTime())
              .count();

      // We normalise on bundle here
      duration = (elapsedSeconds + size * duration) / (size + 1);
    }
    size++;
  }

  _stats->trackConfirmedBundle(totalValue, size, duration, _counters, _histograms);

  _unconfirmedBundles.erase(entry);
  _confirmedBundles.push_back(msg->bundle());
  if (_confirmedBundles.size() > MAX_CONFIRMED_BUNDLES_TO_KEEP) {
    _confirmedBundles.pop_front();
  }
}
}  // namespace statscollector
}  // namespace tanglescope
}  // namespace iota
