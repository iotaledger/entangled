#include <iostream>
#include <string>

#include <rx.hpp>
#include <zmq.hpp>

#include <statscollector/iri.hpp>

int main() {
  auto zmqThread = rxcpp::schedulers::make_new_thread();
  auto zmqObservable =
      rxcpp::observable<>::create<std::string>(
          [](rxcpp::subscriber<std::string> s) {
            std::array<char, 2048> buf;
            int size;

            //  Connect our subscriber socket
            zmq::context_t context(1);
            zmq::socket_t subscriber(context, ZMQ_SUB);
            subscriber.setsockopt(ZMQ_IDENTITY, "statscollector", 5);
            subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
            subscriber.connect("tcp://m5.iotaledger.net:5556");

            while (1) {
              buf.fill('\0');
              size = zmq_recv(subscriber, buf.data(), buf.size() - 1, 0);
              if (size == -1)
                continue;

              std::string str = strdup(buf.data());
              s.on_next(std::move(str));
            }

            s.on_completed();
          })
          .observe_on(rxcpp::observe_on_new_thread());


  zmqObservable.subscribe_on(rxcpp::observe_on_event_loop())
      .as_blocking()
      .subscribe([](std::string str) { std::cout << str << std::endl; },
                 []() {});

  return 0;
}
