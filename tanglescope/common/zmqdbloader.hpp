#pragma once

#include <chrono>
#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>
#include <rx.hpp>

#include "tanglescope/common/iri.hpp"

namespace iota {
namespace tanglescope {

class ZmqDBLoader {
 public:
  void start();
  bool parseConfiguration(const YAML::Node& conf);

  constexpr static auto PUBLISHER = "publisher";
  constexpr static auto OLDEST_TX_AGE = "oldest_tx_age";
  constexpr static auto CLEANUP_INTERVAL = "cleanup_interval";

 private:
  virtual void loadDB();
  void cleanDBPeriodically();

  std::string _zmqPublisherURL;
  uint32_t _oldestTXAge;
  uint32_t _cleanupInterval;
};

}  // namespace tanglescope
}  // namespace iota
