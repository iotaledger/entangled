#include "api.hpp"

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <glog/logging.h>

namespace iota {
namespace utils {
namespace api {

pplx::task<GetTransactionsToApproveResponse>
IRIClient::getTransactionsToApprove(int depth) {
  using namespace web;
  using namespace web::http;

  client::http_client client(_url);
  http_request req(methods::POST);
  setupRequest(req);

  auto body = json::value::object();
  body["command"] = json::value::string("getTransactionsToApprove");
  body["depth"] = json::value::number(depth);

  req.set_body(std::move(body));

  return client.request(std::move(req))
      .then([](http_response response) { return response.extract_json(); })
      .then([](json::value js) {
        return GetTransactionsToApproveResponse{
            js["trunkTransaction"].as_string(),
            js["branchTransaction"].as_string(),
        };
      });
}

pplx::task<web::json::value> IRIClient::broadcastTransactions(
    const std::vector<std::string>& txs) {
  using namespace web;
  using namespace web::http;

  client::http_client client(_url);
  http_request req(methods::POST);
  setupRequest(req);

  auto body = json::value::object();
  body["command"] = json::value::string("broadcastTransactions");

  std::vector<json::value> values(txs.size());
  std::transform(txs.begin(), txs.end(), values.begin(),
                 [](std::string str) { return json::value::string(str); });

  auto txArr = json::value::array(std::move(values));

  body["trytes"] = std::move(txArr);

  req.set_body(std::move(body));

  return client.request(std::move(req)).then([](http_response response) {
    return response.extract_json();
  });
}

void IRIClient::setupRequest(web::http::http_request& req) {
  req.headers().add("X-IOTA-API-Version", "1");
}

}  // namespace api
}  // namespace utils
}  // namespace iota
