/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/asio.hpp>

#include "gossip/iota_packet.h"
#include "gossip/services/receiver.h"
#include "gossip/services/udp_sender.hpp"
#include "utils/logger_helper.h"

bool udp_endpoint_init(endpoint_t *const endpoint) {
  if (endpoint == NULL) {
    return false;
  }

  try {
    boost::asio::io_context ctx;
    boost::asio::ip::udp::resolver resolver(ctx);
    boost::asio::ip::udp::resolver::query query(endpoint->host,
                                                std::to_string(endpoint->port));
    boost::asio::ip::udp::endpoint destination = *resolver.resolve(query);
    endpoint->opaque_inetaddr = new boost::asio::ip::udp::endpoint(destination);
    strcpy(endpoint->ip, destination.address().to_string().c_str());
  } catch (std::exception const &e) {
    log_error("Initiliazing endpoint udp://%s:%d failed: %s", endpoint->host,
              endpoint->port, e.what());
    return false;
  }
  return true;
}

bool udp_endpoint_destroy(endpoint_t *const endpoint) {
  if (endpoint == NULL) {
    return false;
  }

  boost::asio::ip::udp::endpoint *destination =
      reinterpret_cast<boost::asio::ip::udp::endpoint *>(
          endpoint->opaque_inetaddr);
  delete destination;
  return true;
}

bool udp_send(receiver_service_t *const service, endpoint_t *const endpoint,
              iota_packet_t *const packet) {
  if (service == NULL || service->opaque_socket == NULL || endpoint == NULL ||
      endpoint->opaque_inetaddr == NULL) {
    return false;
  }

  try {
    auto socket = reinterpret_cast<boost::asio::ip::udp::socket *>(
        service->opaque_socket);
    socket->async_send_to(
        boost::asio::buffer(packet->content, PACKET_SIZE),
        *reinterpret_cast<boost::asio::ip::udp::endpoint *>(
            endpoint->opaque_inetaddr),
        [](const boost::system::error_code &, std::size_t) {});
  } catch (std::exception const &e) {
    log_error("Sending packet to udp://%s:%d failed: %s", endpoint->host,
              endpoint->port, e.what());
    return false;
  }
  return true;
}
