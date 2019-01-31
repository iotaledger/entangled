#include "tanglescope/collector/collector.hpp"

namespace iota {
namespace tanglescope {
bool CollectorBase::parseConfiguration(const YAML::Node &conf) {
  if (!conf) {
    return false;
  }
  return true;
}
}  // namespace tanglescope
}  // namespace iota
