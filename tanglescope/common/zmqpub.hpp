#pragma once

#include <atomic>
#include <memory>

#include <rx.hpp>

#include "tanglescope/common/iri.hpp"

namespace iota {
namespace tanglescope {
void zmqPublisher(rxcpp::subscriber<std::shared_ptr<iri::IRIMessage>>, const std::string& uri,
                  const std::atomic<bool>& shouldFinish = false);
}  // namespace tanglescope
}  // namespace iota
