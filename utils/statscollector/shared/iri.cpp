#include "iri.hpp"

#include <string_view>

namespace iri {
std::shared_ptr<iri::IRIMessage> payloadToMsg(std::string_view payload)
{
  auto idx = payload.find(' ');
  auto what = payload.substr(0, idx);
  auto actual = payload.substr(idx + 1);

  if (what == "tx") {
    return std::make_shared<iri::TXMessage>(actual);
  } else if (what == "sn") {
    return std::make_shared<iri::SNMessage>(actual);
  }

  return nullptr;
}
}
