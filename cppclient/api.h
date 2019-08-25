/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CPPCLIENT_API_H_
#define CPPCLIENT_API_H_

#include <chrono>
#include <cstdint>
#include <nonstd/optional.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "cppclient/messages.h"

namespace cppclient {

struct Transaction {
 public:
  std::string hash;
  std::string address;
  int64_t value;
  std::chrono::system_clock::time_point timestamp;
  int64_t currentIndex;
  int64_t lastIndex;
  std::string bundleHash;
  std::string trunk;
  nonstd::optional<std::string> message;
};

struct NodeInfo {
 public:
  std::string latestMilestone;
  uint64_t latestMilestoneIndex;
  uint64_t latestSolidMilestoneIndex;
};

using Bundle = std::vector<Transaction>;

/// IotaAPI class.
/// Provides an API to the tangle. The following API are available:
/// - isNodeSolid
/// - getBalances
/// - getConfirmedBundlesForAddress
/// - filterConfirmedTails
/// - filterConsistentTails
/// - findTransactions
/// - getNodeInfo
/// - getTransactions
/// - attachToTangle
/// - getTransactionsToApprove
/// - storeTransactions
/// - broadcastTransactions
class IotaAPI {
 public:
  explicit IotaAPI(bool useSsl) { _useSsl = useSsl; }
  bool isSsl() const { return _useSsl; }

  virtual bool isNodeSolid() = 0;

  virtual nonstd::optional<std::unordered_map<std::string, uint64_t>> getBalances(
      const std::vector<std::string>& addresses) = 0;

  virtual std::unordered_multimap<std::string, Bundle> getConfirmedBundlesForAddresses(
      const std::vector<std::string>& addresses, bool includeMessages) = 0;

  virtual std::unordered_set<std::string> filterConfirmedTails(const std::vector<std::string>& tails,
                                                               const nonstd::optional<std::string>& reference) = 0;

  virtual std::unordered_set<std::string> filterConsistentTails(const std::vector<std::string>& tails) = 0;

  virtual std::vector<std::string> findTransactions(nonstd::optional<std::vector<std::string>> addresses,
                                                    nonstd::optional<std::vector<std::string>> bundles,
                                                    nonstd::optional<std::vector<std::string>> approvees) = 0;

  virtual nonstd::optional<NodeInfo> getNodeInfo() = 0;

  virtual std::vector<Transaction> getTransactions(const std::vector<std::string>& hashes, bool includeMessages) = 0;

  virtual std::vector<std::string> getTrytes(const std::vector<std::string>& hashes) = 0;

  virtual std::vector<std::string> attachToTangle(const std::string& trunkTransaction,
                                                  const std::string& branchTransaction, size_t minWeightMagnitude,
                                                  const std::vector<std::string>& trytes) = 0;

  virtual nonstd::optional<GetTransactionsToApproveResponse> getTransactionsToApprove(
      uint32_t depth, const nonstd::optional<std::string>& reference = {}) = 0;

  virtual bool storeTransactions(const std::vector<std::string>& trytes) = 0;

  virtual nonstd::optional<GetInclusionStatesResponse> getInclusionStates(const std::vector<std::string>& trans,
                                                                          const std::vector<std::string>& tips) = 0;

  virtual bool broadcastTransactions(const std::vector<std::string>& trytes) = 0;

  virtual nonstd::optional<WereAddressesSpentFromResponse> wereAddressesSpentFrom(
      const std::vector<std::string>& addresses) = 0;

 private:
  bool _useSsl;
  std::string _sslPemPath;
};
}  // namespace cppclient

#endif  // CPPCLIENT_API_H_
