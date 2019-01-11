/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/asio.hpp>

#include "gossip/iota_packet.h"
#include "gossip/services/receiver.h"
#include "gossip/services/tcp_sender.hpp"

bool tcp_send(receiver_service_t *const service, endpoint_t *const endpoint,
              iota_packet_t const *const packet) {
  if (endpoint == NULL || endpoint->opaque_inetaddr == NULL) {
    return false;
  }
  try {
    auto socket = reinterpret_cast<boost::asio::ip::tcp::socket *>(
        endpoint->opaque_inetaddr);
    boost::asio::async_write(
        *socket, boost::asio::buffer(packet->content, PACKET_SIZE),
        [](const boost::system::error_code &, std::size_t) {});
  } catch (std::exception const &e) {
    return false;
  }
  return true;
}
