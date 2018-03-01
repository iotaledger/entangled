#include <chrono>
#include <memory>
#include <mutex>

#include <glog/logging.h>

#include "iota/utils/common/iri.hpp"
#include "iota/utils/statscollector/stats/frame.hpp"

namespace iota {
namespace utils {
namespace statscollector {

FrameTXStats::FrameTXStats(){
_frame = std::make_unique<StatsFrame>();
}

std::unique_ptr<StatsFrame> FrameTXStats::swapFrame() {
  std::lock_guard guard(_mutex);

  auto old = std::move(_frame);
  _frame = std::make_unique<StatsFrame>();

  return old;
}


void FrameTXStats::trackNewTX(iri::TXMessage& tx) {
  std::lock_guard guard(_mutex);

  VLOG(5) << "trackNewTX({" << tx.value() << ", "
          << std::chrono::duration_cast<std::chrono::milliseconds>(
                 tx.arrivalTime().time_since_epoch())
                 .count()
          << "})";
  _frame->transactionsNew++;
  if (tx.value() > 0) {
    _frame->valueNew += static_cast<int64_t>(tx.value());
  }
}
void FrameTXStats::trackReattachedTX() {
  std::lock_guard guard(_mutex);

  VLOG(5) << "trackReattachedTX";
  _frame->transactionsReattached++;
}

void FrameTXStats::trackNewBundle() {
  std::lock_guard guard(_mutex);

  VLOG(5) << "trackNewBundle()";
  _frame->bundlesNew++;
}
void FrameTXStats::trackConfirmedBundle(int64_t totalValue, uint64_t size,
                                        uint64_t avgBundleDuration) {
  std::lock_guard guard(_mutex);

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
}
}
}
