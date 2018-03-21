#pragma once

#include <iota/utils/exposer/exposer.hpp>
#include <list>
#include <string>

namespace iota {
namespace utils {

class EchoCatcher : public StatsExposer {
 public:
  void expose() override;
  bool parseConfiguration(const YAML::Node& conf) override;

 private:
  std::string _prometheusExpURI;
  std::string _iriHost;
  std::list<std::string> _zmqPublishers;
  uint32_t _tangleDBWarmupPeriod;
  uint32_t _mwm;
};

}  // namespace utils
}  // namespace iota
