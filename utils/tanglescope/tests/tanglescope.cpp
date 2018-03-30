#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <iota/utils/echocatcher/echocatcher.hpp>
#include <iota/utils/statscollector.hpp>

using namespace iota::utils::statscollector;
using namespace iota::utils;

class MockEchoCatcher : public EchoCatcher {
 public:
  MOCK_METHOD0(loadDB, void());
  MOCK_METHOD0(broadcastTransactions, void());
  MOCK_METHOD0(handleReceivedTransactions,void());
};

TEST(TangleScope, EchoCatcherFlow) {
  ::google::InitGoogleLogging("echocatcher test");

  using namespace testing;
  YAML::Node config;
  config["iri_host"] = "some_host";
  config["publishers"].push_back("p1");
  config["publishers"].push_back("p2");
  config["tangledb_warmup_period"] = "100";
  config["prometheus_exposer_uri"] = "some_exposer_uri";
  config["mwm"] = "17";
  config["broadcast_interval"] = "10";
  auto mockEchoCatcher = std::make_shared<MockEchoCatcher>();

  ASSERT_EQ(mockEchoCatcher->parseConfiguration(config), true);

  EXPECT_CALL(*(mockEchoCatcher), loadDB()).Times(1);
  EXPECT_CALL(*(mockEchoCatcher), broadcastTransactions()).Times(1);
  EXPECT_CALL(*(mockEchoCatcher), handleReceivedTransactions()).Times(1);


  mockEchoCatcher->collect();
}
