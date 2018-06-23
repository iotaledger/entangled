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

using namespace testing;
using namespace cppclient;

using json = nlohmann::json;

namespace {

class IotaJsonAPITest : public ::testing::Test {};

class MockAPI : public IotaJsonAPI {
 public:
  json req;
  json res;

  MOCK_METHOD1(post, nonstd::optional<json>(const json&));
};

TEST_F(IotaJsonAPITest, GetBalances) {
  MockAPI api;

  std::string address(
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAA");
  uint64_t balance = 1000;

  std::vector<std::string> addresses = {address};
  std::unordered_map<std::string, uint64_t> expected = {{address, 1000}};

  json req;
  req["command"] = "getBalances";
  req["threshold"] = 100;
  req["addresses"] = addresses;
  json res;
  res["balances"] = std::vector<std::string>{"1000"};

  EXPECT_CALL(api, post(req)).Times(1).WillOnce(Return(res));

  auto response = api.getBalances(addresses);

  EXPECT_EQ(response, expected);
}

};  // namespace
