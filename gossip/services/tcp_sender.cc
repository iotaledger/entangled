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
#include "utils/logger_helper.h"

bool tcp_send(receiver_service_t *const service, endpoint_t *const endpoint,
              iota_packet_t *const packet) {
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
    log_error("Sending packet to tcp://%s:%d failed: %s", endpoint->host,
              endpoint->port, e.what());
    return false;
  }
  return true;
}
