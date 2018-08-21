/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/asio.hpp>

#include "common/network/iota_packet.h"
#include "common/network/services/udp_sender.hpp"
#include "utils/logger_helper.h"

bool udp_send(endpoint_t *const endpoint, iota_packet_t *const packet) {
  if (endpoint == NULL || endpoint->opaque_inetaddr == NULL) {
    return false;
  }
  try {
    auto socket = reinterpret_cast<boost::asio::ip::udp::socket *>(
        endpoint->opaque_inetaddr);
    boost::asio::ip::udp::endpoint destination(
        boost::asio::ip::address::from_string(endpoint->host), endpoint->port);
    socket->async_send_to(
        boost::asio::buffer(packet->content, PACKET_SIZE), destination,
        [](const boost::system::error_code &, std::size_t) {});
  } catch (std::exception const &e) {
    log_error("Sending packet to udp://%s:%d failed: %s", endpoint->host,
              endpoint->port, e.what());
    return false;
  }
  return true;
}
