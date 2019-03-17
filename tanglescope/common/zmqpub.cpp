#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>

#include <rx.hpp>

#include <zmq.hpp>

#include "iri.hpp"
#include "zmqpub.hpp"

namespace iota {
namespace tanglescope {

void zmqPublisher(rxcpp::subscriber<std::shared_ptr<iri::IRIMessage>> s, const std::string& uri,
                  const std::atomic<bool>& shouldFinish) {
  std::array<char, 2048> buf;

  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_SUB);
  subscriber.setsockopt(ZMQ_IDENTITY, "tanglescope/common", 5);
  subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  subscriber.connect(uri);

  zmq::poller_t<> poller;
  std::vector<zmq_poller_event_t> events(1);

  poller.add(subscriber, ZMQ_POLLIN, nullptr);

  while (!shouldFinish && s.is_subscribed()) {
    buf.fill('\0');

    poller.wait_all(events, std::chrono::milliseconds(-1));

    int size = zmq_recv(subscriber, buf.data(), buf.size() - 1, 0);
    if (size == -1) continue;

    std::string_view view(buf.data(), static_cast<std::string_view::size_type>(size));

    auto msg = iri::payloadToMsg(view);

    if (msg && !shouldFinish) {
      s.on_next(std::move(msg));
    }
  }

  s.on_completed();
}

}  // namespace tanglescope
}  // namespace iota
