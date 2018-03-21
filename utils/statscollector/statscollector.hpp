#pragma once

#include <prometheus/exposer.h>
#include <iota/utils/exposer/exposer.hpp>
#include <set>
#include <string>

namespace iota {
namespace utils {
namespace statscollector {

class StatsCollector : public StatsExposer {
 public:
  void expose() override;
  bool parseConfiguration(const YAML::Node& conf) override;

 private:
  std::map<std::string,
           std::reference_wrapper<prometheus::Family<prometheus::Gauge>>>
  buildMetricsMap(std::shared_ptr<prometheus::Registry> registry);

  std::string _prometheusExpURI;
  std::string _zmqURL;
  uint32_t _pubDelay;
  uint32_t _pubInterval;
};
}  // namespace statscollector

}  // namespace utils
}  // namespace iota
