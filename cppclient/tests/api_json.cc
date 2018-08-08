// Copyright 2018 IOTA Foundation

#include <chrono>
#include <iostream>
#include <nonstd/optional.hpp>
#include <thread>

#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "cppclient/api_json.h"

using cppclient::IotaJsonAPI;
using testing::Return;

using json = nlohmann::json;

namespace {

class IotaJsonAPITest : public ::testing::Test {};

class MockAPI : public IotaJsonAPI {
 public:
  json request;
  nonstd::optional<json> response;

  MOCK_METHOD0(post_, void(void));

  nonstd::optional<json> post(const json& in) {
    request = in;
    post_();
    return response;
  }
};

TEST_F(IotaJsonAPITest, GetBalances) {
  MockAPI api;

  std::string address(
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAA");
  uint64_t balance = 1000;

  std::vector<std::string> addresses = {address};
  std::unordered_map<std::string, uint64_t> expected = {{address, balance}};

  json req;
  req["command"] = "getBalances";
  req["threshold"] = 100;
  req["addresses"] = addresses;
  json res;
  res["balances"] = std::vector<std::string>{"1000"};


  api.response = res;
  EXPECT_CALL(api, post_()).Times(1);

  auto response = api.getBalances(addresses);

  EXPECT_EQ(api.request, req);
  EXPECT_EQ(response, expected);
}

TEST_F(IotaJsonAPITest, GetNodeInfo) {
  MockAPI api;

  json req;
  req["command"] = "getNodeInfo";
  json res;
  res["latestMilestone"] = "VBVEUQYE99LFWHDZRFKTGFHYGDFEAMAEBGUBTTJRFKHCFBRTXFAJQ9XIUEZQCJOQTZNOOHKUQIKOY9999";
  res["latestMilestoneIndex"] = 107;
  res["latestSolidSubtangleMilestoneIndex"] = 107;
  
  cppclient::NodeInfo expected = {res["latestMilestone"], res["latestMilestoneIndex"]
                              , res["latestSolidSubtangleMilestoneIndex"]}; 
  
  api.response = res;

  EXPECT_CALL(api, post_()).Times(1);  
  
  auto response = api.getNodeInfo();
  
  EXPECT_EQ(api.request, req);
  EXPECT_EQ((*response).latestMilestone, res["latestMilestone"]);
  EXPECT_EQ((*response).latestMilestoneIndex, res["latestMilestoneIndex"]);
  EXPECT_EQ((*response).latestSolidMilestoneIndex, res["latestSolidSubtangleMilestoneIndex"]);

}

};  // namespace
