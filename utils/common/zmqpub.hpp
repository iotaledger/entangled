#pragma once

#include <iota/utils/common/iri.hpp>
#include <memory>
#include <atomic>
#include <rx.hpp>

namespace iota {
namespace utils {
void zmqPublisher(rxcpp::subscriber<std::shared_ptr<iri::IRIMessage>>,
                  const std::string& uri, const std::atomic<bool>& shouldFinish = false);
}  // namespace utils
}  // namespace iota
