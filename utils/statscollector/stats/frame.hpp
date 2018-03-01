#pragma once

#include <cstdint>
#include <memory>
#include <shared_mutex>

#include "iota/utils/common/iri.hpp"

#include "stats.hpp"

using namespace iota::utils;

namespace iota {
namespace utils {
namespace statscollector {

class StatsFrame {
 public:
  uint64_t transactionsNew;
  uint64_t transactionsReattached;
  uint64_t transactionsConfirmed;
  uint64_t bundlesNew;
  uint64_t bundlesConfirmed;
  uint64_t avgConfirmationDuration;
  uint64_t valueNew;
  uint64_t valueConfirmed;
};

class FrameTXStats : public TXStats {
 public:
  FrameTXStats();
  void trackNewTX(iri::TXMessage &);
  void trackReattachedTX();
  void trackNewBundle();
  void trackConfirmedBundle(int64_t totalValue, uint64_t size,
                            uint64_t avgBundleDuration);

 public:
  std::unique_ptr<StatsFrame> swapFrame();
 private:
  std::shared_mutex _mutex;
  std::unique_ptr<StatsFrame> _frame;
};
}
}
}
