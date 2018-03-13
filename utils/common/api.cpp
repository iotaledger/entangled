#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <glog/logging.h>
#include <chrono>

#include "api.hpp"

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

  auto txArr = stringVectorToStringJsonArray(txs);

  body["trytes"] = std::move(txArr);

  req.set_body(std::move(body));

  return client.request(std::move(req)).then([](http_response response) {
    return response.extract_json();
  });
}

pplx::task<GetNodeInfoResponse>
IRIClient::getNodeInfo() {
    using namespace web;
    using namespace web::http;

    client::http_client client(_url);
    http_request req(methods::POST);
    setupRequest(req);

    auto body = json::value::object();
    body["command"] = json::value::string("getNodeInfo");

    req.set_body(std::move(body));

    return client.request(std::move(req))
            .then([](http_response response) { return response.extract_json(); })
            .then([](json::value js) {
                return GetNodeInfoResponse{
                        js["appName"].as_string(),
                        js["appVersion"].as_string(),
                        js["jreAvailableProcessors"].as_number().to_uint32(),
                        js["jreFreeMemory"].as_number().to_int64(),
                        js["jreVersion"].as_string(),
                        js["jreMaxMemory"].as_number().to_int64(),
                        js["jreTotalMemory"].as_number().to_int64(),
                        js["latestMilestone"].as_string(),
                        js["latestMilestoneIndex"].as_number().to_uint32(),
                        js["latestSolidSubtangleMilestone"].as_string(),
                        js["latestSolidSubtangleMilestoneIndex"].as_number().to_uint32(),
                        js["neighbors"].as_number().to_uint32(),
                        js["packetsQueueSize"].as_number().to_uint32(),
                        std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(js["time"].as_number().to_int64())),
                        js["tips"].as_number().to_uint32(),
                        js["transactionsToRequest"].as_number().to_uint32(),

                };
            });
}

pplx::task<GetInclusionStatesResponse> IRIClient::getInclusionStates(
    const std::vector<std::string>& txs,const std::vector<std::string>& tips){
    using namespace web;
    using namespace web::http;

    client::http_client client(_url);
    http_request req(methods::POST);
    setupRequest(req);

    auto body = json::value::object();
    body["command"] = json::value::string("getInclusionStates");
    auto txArr = stringVectorToStringJsonArray(txs);
    body["transactions"] = std::move(txArr);
    auto tipsArr = stringVectorToStringJsonArray(tips);
    body["tips"] = std::move(tipsArr);

    req.set_body(std::move(body));

    return client.request(std::move(req))
            .then([](http_response response) { return response.extract_json(); })
            .then([](json::value js) {
                GetInclusionStatesResponse response;
                if (js["states"].is_array()){
                    auto arr = js["states"].as_array();
                    for (auto arrIt = arr.begin();arrIt != arr.end(); ++arrIt){
                        response.states.push_back((*arrIt).as_bool());
                    }
                } else{
                    response.error = js["error"].as_string();
                }

                return response;
            });

    }

pplx::task<std::vector<std::string>>
    IRIClient::getTrytes(const std::vector<std::string>& trans){

    using namespace web;
    using namespace web::http;

    client::http_client client(_url);
    http_request req(methods::POST);
    setupRequest(req);

    auto body = json::value::object();
    body["command"] = json::value::string("getTrytes");
    auto transArr = stringVectorToStringJsonArray(trans);
    body["hashes"] = std::move(transArr);

    req.set_body(std::move(body));

    return client.request(std::move(req))
            .then([](http_response response) { return response.extract_json(); })
            .then([](json::value js) {
                std::vector<std::string> trytes;
                auto arr = js["trytes"].as_array();
                for (auto arrIt = arr.begin();arrIt != arr.end(); ++arrIt){
                    trytes.push_back(std::move((*arrIt).as_string()));
                }
                return trytes;
            });

    }

void IRIClient::setupRequest(web::http::http_request& req) {
  req.headers().add("X-IOTA-API-Version", "1");
}

web::json::value IRIClient::stringVectorToStringJsonArray(const std::vector<std::string>& strVec){
  using namespace web;
  using namespace web::http;
  std::vector<json::value> values(strVec.size());
  std::transform(strVec.begin(), strVec.end(), values.begin(),
                 [](std::string str) { return json::value::string(str); });

  return std::move(json::value::array(std::move(values)));
}

}  // namespace api
}  // namespace utils
}  // namespace iota
