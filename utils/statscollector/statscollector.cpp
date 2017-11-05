#include <iostream>
#include <string>

#include <zmq.hpp>

using namespace std;

int main() {
  char buf[1024];
  int size;

  //  Connect our subscriber socket
  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_SUB);
  subscriber.setsockopt(ZMQ_IDENTITY, "statscollector", 5);
  subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  subscriber.connect("tcp://m5.iotaledger.net:5556");

  while (1) {
    size = zmq_recv(subscriber, buf, 1023, 0);
    if (size == -1)
      continue;

    string str = strdup(buf);
    cout << str << endl;
  }

  return 0;
}
