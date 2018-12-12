/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/asio.hpp>
#include <boost/crc.hpp>
#include <iomanip>

#include "gossip/iota_packet.h"
#include "gossip/services/receiver.h"
#include "gossip/services/tcp_sender.hpp"

retcode_t tcp_sender_endpoint_init(endpoint_t *const endpoint) {
  if (endpoint == NULL) {
    return RC_NULL_PARAM;
  }

  try {
    boost::asio::io_context ctx;
    boost::asio::ip::tcp::resolver resolver(ctx);
    boost::asio::ip::tcp::resolver::query query(endpoint->host,
                                                std::to_string(endpoint->port));
    boost::asio::ip::tcp::endpoint destination = *resolver.resolve(query);
    strcpy(endpoint->ip, destination.address().to_string().c_str());
  } catch (std::exception const &e) {
    return RC_NEIGHBOR_FAILED_ENDPOINT_INIT;
  }

  return RC_OK;
}

retcode_t tcp_sender_endpoint_destroy(endpoint_t *const endpoint) {
  return RC_OK;
}

bool tcp_send(receiver_service_t *const service, endpoint_t *const endpoint,
              iota_packet_t const *const packet) {
  if (endpoint == NULL) {
    return false;
  } else if (endpoint->opaque_inetaddr == NULL) {
    return true;
  }

  try {
    auto socket = reinterpret_cast<boost::asio::ip::tcp::socket *>(
        endpoint->opaque_inetaddr);
    boost::system::error_code ignored_error;
    boost::crc_32_type result;
    result.process_bytes(packet->content, PACKET_SIZE);
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(16) << std::hex
           << result.checksum();
    boost::asio::write(*socket, boost::asio::buffer(packet->content),
                       ignored_error);
    boost::asio::write(*socket, boost::asio::buffer(stream.str()),
                       ignored_error);
  } catch (std::exception const &e) {
    return false;
  }
  return true;
}
