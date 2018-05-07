#pragma once

#include <string>
#include <vector>

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <optional>

namespace iota {
namespace tanglescope {
namespace api {

struct GetTransactionsToApproveResponse {
  std::string trunkTransaction;
  std::string branchTransaction;
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
  std::optional<std::string> error;
};

class IRIClient {
 public:
  explicit IRIClient(const std::string& url) : _url(url) {}

  pplx::task<GetTransactionsToApproveResponse> getTransactionsToApprove(
      int depth = 3);

  pplx::task<web::json::value> broadcastTransactions(
      const std::vector<std::string>&);

  pplx::task<GetNodeInfoResponse> getNodeInfo();

  pplx::task<GetInclusionStatesResponse> getInclusionStates(
      const std::vector<std::string>& trans,
      const std::vector<std::string>& tips);

  pplx::task<std::vector<std::string>> getTrytes(
      const std::vector<std::string>& trans);

 protected:
  void setupRequest(web::http::http_request&);
  web::json::value stringVectorToStringJsonArray(
      const std::vector<std::string>&);

 private:
  const std::string _url;
};
}  // namespace api
}  // namespace tanglescope
}  // namespace iota
