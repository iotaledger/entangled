#pragma once

#include <optional>
#include <shared_mutex>
#include <unordered_map>

class TangleDB {
 public:
  struct TXRecord {
    std::string hash;
    std::string trunk;
    std::string branch;

    TXRecord() = default;
  };

  std::optional<TXRecord> find(const std::string& hash);

  void put(const TXRecord& tx);

  static TangleDB& instance();

 private:
  mutable std::shared_mutex mutex_;
  std::unordered_map<std::string, TXRecord> _txs;

  TangleDB() = default;
  ~TangleDB() = default;

  TangleDB(const TangleDB&) = delete;
  TangleDB& operator=(const TangleDB&) = delete;
  TangleDB(TangleDB&&) = delete;
  TangleDB& operator=(TangleDB&&) = delete;
};
