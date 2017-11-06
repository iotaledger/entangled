#pragma once

#include <string>
#include <vector>

#include <cpprest/http_client.h>
#include <cpprest/json.h>

namespace iota {
namespace utils {
namespace api {

struct GetTransactionsToApproveResponse {
  std::string trunkTransaction;
  std::string branchTransaction;
};

class IRIClient {
 public:
  explicit IRIClient(const std::string& url) : _url(url) {}

  pplx::task<GetTransactionsToApproveResponse> getTransactionsToApprove(
      int depth = 3);

  pplx::task<web::json::value> broadcastTransactions(const std::vector<std::string>&);

protected:
  void setupRequest(web::http::http_request&);

 private:
  const std::string _url;
};
}  // namespace api
}  // namespace utils
}  // namespace iota
