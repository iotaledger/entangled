#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <chrono>
#include <cstdint>
#include <memory>
#include <shared_mutex>

#include "iri.hpp"

namespace iota {
namespace utils {
namespace stats {
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

class UnconfirmedTX {
 public:
  explicit UnconfirmedTX(const std::shared_ptr<iri::TXMessage>& msg)
      : value(msg->value()), arrivalTime(msg->arrivalTime()) {}

  const int64_t value;
  const std::chrono::system_clock::time_point arrivalTime;
};

class StatsCollector {
 public:
  explicit StatsCollector() : _frame(std::make_unique<StatsFrame>()) {}

  std::unique_ptr<StatsFrame> swapFrame();

  void onNewTransaction(std::shared_ptr<iri::TXMessage>);
  void onTransactionConfirmed(std::shared_ptr<iri::SNMessage>);

 protected:
  void trackNewTX(UnconfirmedTX&);
  void trackReattachedTX();
  void trackNewBundle();
  void trackConfirmedBundle(int64_t totalValue, uint64_t size,
                            uint64_t avgBundleDuration);

 private:
  std::unordered_set<std::string> _confirmedBundles;
  std::unordered_map<std::string, std::vector<std::unique_ptr<UnconfirmedTX>>>
      _unconfirmedBundles;

  std::shared_mutex _mutex;
  std::unique_ptr<StatsFrame> _frame;
};
}  // namespace stats
}  // namespace utils
}  // namespace iota
