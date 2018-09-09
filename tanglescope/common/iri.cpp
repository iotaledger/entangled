#include <string_view>

#include "iri.hpp"

namespace iota {
namespace tanglescope {
namespace iri {

std::shared_ptr<IRIMessage> payloadToMsg(std::string_view payload) {
  auto idx = payload.find(' ');
  auto what = payload.substr(0, idx);
  auto actual = payload.substr(idx + 1);

  if (what == "tx") {
    return std::make_shared<TXMessage>(actual);
  } else if (what == "sn") {
    return std::make_shared<SNMessage>(actual);
  } else if (what == "lmhs") {
    return std::make_shared<LMHSMessage>(actual);
  } else if (what == "lmsi") {
    return std::make_shared<LMSIMessage>(actual);
  } else if (what == "rstat") {
    return std::make_shared<RSTATMessage>(actual);
  }

  return nullptr;
}
}  // namespace iri
}  // namespace tanglescope
}  // namespace iota
