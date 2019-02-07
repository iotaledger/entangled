#include <glog/logging.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include "tanglescope/echocollector.hpp"
#include "tanglescope/statscollector/statscollector.hpp"

using namespace iota::tanglescope::statscollector;
using namespace iota::tanglescope;

class MockEchoCollector : public EchoCollector {
 public:
  MOCK_METHOD0(loadDB, void());
  MOCK_METHOD0(broadcastTransactions, void());
  MOCK_METHOD0(handleReceivedTransactions, void());
};

TEST(TangleScope, EchoCollectorFlow) {
  ::google::InitGoogleLogging("echocatcher test");

  using namespace testing;
  YAML::Node config;
  config[EchoCollector::IRI_HOST] = "some_host";
  config[EchoCollector::IRI_PORT] = "1234";
  config[EchoCollector::PUBLISHERS].push_back("p1");
  config[EchoCollector::PUBLISHERS].push_back("p2");
  config[PrometheusCollector::PROMETHEUS_EXPOSER_URI] = "some_exposer_uri";
  config[EchoCollector::MWM] = "17";
  config[EchoCollector::DISCOVERY_INTERVAL] = "10";
  config[EchoCollector::BROADCAST_INTERVAL] = "10";

  auto mockEchoCollector = std::make_shared<MockEchoCollector>();

  ASSERT_EQ(mockEchoCollector->parseConfiguration(config), true);

  EXPECT_CALL(*(mockEchoCollector), loadDB()).Times(1);
  EXPECT_CALL(*(mockEchoCollector), broadcastTransactions()).Times(1);
  EXPECT_CALL(*(mockEchoCollector), handleReceivedTransactions()).Times(1);

  mockEchoCollector->collect();
}
