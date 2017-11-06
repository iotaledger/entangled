#include "iri.hpp"

#include <string_view>

namespace iota {
namespace utils {
namespace iri {

std::shared_ptr<IRIMessage> payloadToMsg(std::string_view payload) {
  auto idx = payload.find(' ');
  auto what = payload.substr(0, idx);
  auto actual = payload.substr(idx + 1);

  if (what == "tx") {
    return std::make_shared<TXMessage>(actual);
  } else if (what == "sn") {
    return std::make_shared<SNMessage>(actual);
  }

  return nullptr;
}
}  // namespace iri
}  // namespace utils
}  // namespace iota
