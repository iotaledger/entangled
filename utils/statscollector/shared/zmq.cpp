#include <iostream>
#include <map>
#include <string>
#include <string_view>

#include <rx.hpp>
#include <zmq.hpp>

#include "./zmq.hpp"

#include <statscollector/iri.hpp>

void zmqPublisher(rxcpp::subscriber<std::shared_ptr<iri::IRIMessage>> s, const std::string& uri)
{
  std::array<char, 2048> buf;

  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_SUB);
  subscriber.setsockopt(ZMQ_IDENTITY, "statscollector", 5);
  subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  subscriber.connect(uri);

  while (1) {
    buf.fill('\0');
    int size = zmq_recv(subscriber, buf.data(), buf.size() - 1, 0);
    if (size == -1)
      continue;

    auto msg = iri::payloadToMsg({ buf.data(), static_cast<std::string_view::size_type>(size) + 1 });

    if (msg) {
      s.on_next(std::move(msg));
    }
  }

  s.on_completed();
}

