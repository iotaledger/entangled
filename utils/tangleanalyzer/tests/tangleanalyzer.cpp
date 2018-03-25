#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <iota/utils/echocatcher/echocatcher.hpp>
#include <iota/utils/statscollector.hpp>

using namespace iota::utils::statscollector;
using namespace iota::utils;

class MockStatsCollector : public StatsCollector {
 public:
  MOCK_METHOD0(expose, void());
};

class MockEchoCatcher : public EchoCatcher {
 public:
  MOCK_METHOD0(loadDB, void());
  MOCK_METHOD0(broadcastTransactions, EchoCatcher::HashedTX());
  MOCK_METHOD2(handleRecievedTransactions,
               void(EchoCatcher::HashedTX,
                    std::chrono::time_point<std::chrono::system_clock>));
};

TEST(TangleAnalyzer, EchoCatcherFlow) {
  ::google::InitGoogleLogging("echocatcher test");

  using namespace testing;
  YAML::Node config;
  config["iri_host"] = "some_host";
  config["publishers"].push_back("p1");
  config["publishers"].push_back("p2");
  config["tangledb_warmup_period"] = "100";
  config["prometheus_exposer_uri"] = "some_exposer_uri";
  config["mwm"] = "17";
  auto mockEchoCatcher = std::make_shared<MockEchoCatcher>();

  ASSERT_EQ(mockEchoCatcher->parseConfiguration(config), true);

  EXPECT_CALL(*(mockEchoCatcher), loadDB()).Times(1);
  EXPECT_CALL(*(mockEchoCatcher), broadcastTransactions()).Times(1);
  EXPECT_CALL(*(mockEchoCatcher), handleRecievedTransactions(_, _)).Times(1);

  mockEchoCatcher->expose();
}
