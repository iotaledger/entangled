#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>

#include <rx.hpp>

#include <zmq.hpp>

#include "zmqpub.hpp"
#include "iri.hpp"

namespace iota {
namespace utils {

void zmqPublisher(rxcpp::subscriber<std::shared_ptr<iri::IRIMessage>> s,
                  const std::string& uri, const std::atomic<bool>& shouldFinish) {
  std::array<char, 2048> buf;

  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_SUB);
  subscriber.setsockopt(ZMQ_IDENTITY, "iota/utils/common", 5);
  subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  subscriber.connect(uri);

  auto poller = std::make_unique<zmq::poller_t>();

  auto handler = std::function<void(void)>();
  poller->add(subscriber, ZMQ_POLLIN, handler);

  while (!shouldFinish && s.is_subscribed()) {
    buf.fill('\0');

    poller->wait(std::chrono::milliseconds(-1));

    int size = zmq_recv(subscriber, buf.data(), buf.size() - 1, 0);
    if (size == -1) continue;

    std::string_view view(buf.data(),
                          static_cast<std::string_view::size_type>(size));

    auto msg = iri::payloadToMsg(view);

    if (msg && !shouldFinish) {
      s.on_next(std::move(msg));
    }
  }

  s.on_completed();
}



}  // namespace utils
}  // namespace iota
