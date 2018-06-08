#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <iota/tanglescope/echocatcher/echocatcher.hpp>
#include <iota/tanglescope/statscollector.hpp>

using namespace iota::tanglescope::statscollector;
using namespace iota::tanglescope;

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
  config[EchoCatcher::IRI_HOST] = "some_host";
  config[EchoCatcher::IRI_PORT] = "1234";
  config[EchoCatcher::PUBLISHERS].push_back("p1");
  config[EchoCatcher::PUBLISHERS].push_back("p2");
  config[EchoCatcher::TANGLE_DB_WARMUP_TIME] = "100";
  config[PrometheusCollector::PROMETHEUS_EXPOSER_URI] = "some_exposer_uri";
  config[EchoCatcher::MWM] = "17";
  config[EchoCatcher::DISCOVERY_INTERVAL] = "10";
  config[EchoCatcher::BROADCAST_INTERVAL] = "10";

  auto mockEchoCatcher = std::make_shared<MockEchoCatcher>();

  ASSERT_EQ(mockEchoCatcher->parseConfiguration(config), true);

  EXPECT_CALL(*(mockEchoCatcher), loadDB()).Times(1);
  EXPECT_CALL(*(mockEchoCatcher), broadcastTransactions()).Times(1);
  EXPECT_CALL(*(mockEchoCatcher), handleReceivedTransactions()).Times(1);


  mockEchoCatcher->collect();
}
