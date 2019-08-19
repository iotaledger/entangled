/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <chrono>
#include <iostream>
#include <thread>

#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "cppclient/beast.h"

using cppclient::BeastIotaAPI;

using json = nlohmann::json;

namespace {

class BeastIotaAPITest : public ::testing::Test {};

class MockBeast : public BeastIotaAPI {
 public:
  using BeastIotaAPI::BeastIotaAPI;
  using BeastIotaAPI::post;
  explicit MockBeast(std::string host, uint32_t port, bool useSsl) : IotaAPI(false), BeastIotaAPI(host, port, false) {}
};

TEST_F(BeastIotaAPITest, InvalidHostPort) {
  MockBeast beast1("localhost", 1, false);
  MockBeast beast2("unresolvable.sometld", 41414, false);
  json req = R"({"command":"getNodeInfo"})"_json;

  auto res1 = beast1.post(req);
  auto res2 = beast2.post(req);

  EXPECT_FALSE(res1);
  EXPECT_FALSE(res2);
}

};  // namespace
