/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/asio.hpp>

#include "common/network/logger.h"
#include "common/network/services/udp_sender.hpp"

bool udp_send(endpoint_t *const endpoint) {
  if (endpoint == NULL || endpoint->opaque_inetaddr == NULL) {
    return false;
  }
  auto sock = reinterpret_cast<boost::asio::ip::udp::socket *>(
      endpoint->opaque_inetaddr);
  try {
    // TODO(thibault) clear
    boost::asio::ip::address addr;
    addr.from_string(endpoint->host);
    boost::asio::ip::udp::endpoint sender_endpoint(addr, endpoint->port);
    sock->send_to(boost::asio::buffer("Broadcast\n", 10), sender_endpoint);
  } catch (std::exception const &e) {
    log_error("UDP write failed:  %s", e.what());
    return false;
  }
  return true;
}
