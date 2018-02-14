#pragma once

#include <iota/utils/common/api.hpp>
#include <iota/utils/common/iri.hpp>
#include <map>

namespace iota {
namespace utils {

class TXAuxiliary {
 public:
  static TXAuxiliary& instance();

  pplx::task<void> handleUnseenTransactions(
      std::shared_ptr<iri::TXMessage> tx,
      std::map<std::string, std::chrono::system_clock::time_point>& hashToSeenTimestamp,
      std::chrono::time_point<std::chrono::system_clock> received,
      std::weak_ptr<api::IRIClient> iriClient);

 private:
  pplx::task<std::set<std::string>> getUnconfirmedTXs(
      std::weak_ptr<api::IRIClient> client, std::shared_ptr<iri::TXMessage> tx);

  pplx::task<void> removeConfirmedTransactions(
      std::weak_ptr<api::IRIClient> client,
      const std::vector<std::string>& tips, std::vector<std::string>& txs);

 private:
  TXAuxiliary() = default;
  ~TXAuxiliary() = default;

  TXAuxiliary(const TXAuxiliary&) = delete;
  TXAuxiliary& operator=(const TXAuxiliary&) = delete;
  TXAuxiliary(TXAuxiliary&&) = delete;
  TXAuxiliary& operator=(TXAuxiliary&&) = delete;
};

}  // namespace utils
}  // namespace iota
