/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CPPCLIENT_MESSAGES_H_
#define CPPCLIENT_MESSAGES_H_

#include <nonstd/optional.hpp>
#include <string>
#include <vector>

namespace cppclient {

struct GetTransactionsToApproveResponse {
  std::string trunkTransaction;
  std::string branchTransaction;
  uint32_t duration;
};

struct GetNodeInfoResponse {
  std::string appName;
  std::string appVersion;
  uint32_t jreAvailableProcessors;
  uint64_t jreFreeMemory;
  std::string jreVersion;

  uint64_t jreMaxMemory;
  uint64_t jreTotalMemory;
  std::string latestMilestone;
  uint32_t latestMilestoneIndex;

  std::string latestSolidSubtangleMilestone;
  uint32_t latestSolidSubtangleMilestoneIndex;

  uint32_t neighbors;
  uint32_t packetsQueueSize;
  std::chrono::time_point<std::chrono::system_clock> time;
  uint32_t tips;
  uint32_t transactionsToRequest;
};

struct GetInclusionStatesResponse {
  std::vector<bool> states;
};

struct WereAddressesSpentFromResponse {
  std::vector<bool> states;
};
}  // namespace cppclient

#endif  // CPPCLIENT_MESSAGES_H_
