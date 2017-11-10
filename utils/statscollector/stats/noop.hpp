#pragma once

#include "iota/utils/common/iri.hpp"
#include "stats.hpp"

using namespace iota::utils;

namespace iota {
namespace utils {
namespace statscollector {

class NoopTXStats : public TXStats {
 public:
  void trackNewTX(iri::TXMessage &){};
  void trackReattachedTX(){};
  void trackNewBundle(){};
  void trackConfirmedBundle(int64_t totalValue, uint64_t size,
                            uint64_t avgBundleDuration){};
};
}
}
}
