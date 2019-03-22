#include "tangledb.hpp"
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

TangleDB& TangleDB::instance() {
  static TangleDB db;
  return db;
}

nonstd::optional<TangleDB::TXRecord> TangleDB::find(const std::string& hash) {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto tx = _txs.find(hash);
  if (tx != _txs.end()) {
    return tx->second;
  }
  return {};
}

void TangleDB::put(const TXRecord& tx) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  _txs[tx.hash] = tx;
}

void TangleDB::removeAgedTxs(uint32_t ageInSeconds) {
  auto now = std::chrono::system_clock::now();

  std::unique_lock<std::shared_mutex> lock(mutex_);

  for (auto&& it = _txs.begin(); it != _txs.end();) {
    if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second.timestamp).count() > ageInSeconds) {
      _txs.erase(it++);
    } else {
      ++it;
    }
  }
}

std::unordered_map<std::string, TangleDB::TXRecord> TangleDB::getTXsMap() const {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return _txs;
};
