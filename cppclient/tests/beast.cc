// Copyright 2018 IOTA Foundation

#include <chrono>
#include <iostream>
#include <thread>

#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "cppclient/beast.h"

using namespace testing;
using namespace cppclient;


using json = nlohmann::json;

namespace {

class BeastIotaAPITest : public ::testing::Test {};

class MockBeast : public BeastIotaAPI {
 public:
  using BeastIotaAPI::BeastIotaAPI;
  using BeastIotaAPI::post;
};

TEST_F(BeastIotaAPITest, InvalidHostPort) {
  MockBeast beast1("localhost", 1);
  MockBeast beast2("unresolvable.sometld", 41414);
  json req = R"({"command":"getNodeInfo"})"_json;

  auto res1 = beast1.post(req);
  auto res2 = beast2.post(req);

  EXPECT_FALSE(res1);
  EXPECT_FALSE(res2);
}

};  // namespace
