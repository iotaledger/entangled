#pragma once

#include <atomic>
#include <memory>

#include <rx.hpp>

#include "tanglescope/common/iri.hpp"

namespace iota {
namespace tanglescope {
using ZmqObservable = rxcpp::observable<std::shared_ptr<iri::IRIMessage>>;
void zmqPublisher(rxcpp::subscriber<std::shared_ptr<iri::IRIMessage>>, const std::string& uri,
                  const std::atomic<bool>& shouldFinish = false);
static std::map<std::string, ZmqObservable> urlToZmqObservables;
}  // namespace tanglescope
}  // namespace iota
