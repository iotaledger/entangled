#pragma once

#include <yaml-cpp/yaml.h>
#include <string>

namespace iota {
namespace tanglescope {

class CollectorBase {
 public:
  virtual ~CollectorBase(){};
  virtual void collect() = 0;
  virtual bool parseConfiguration(const YAML::Node& conf);
};
}  // namespace tanglescope
}  // namespace iota
