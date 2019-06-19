/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <iomanip>

#include <boost/asio.hpp>
#include <boost/crc.hpp>

#include "node/iota_packet.h"
#include "node/services/receiver.h"
#include "node/services/tcp_sender.hpp"

retcode_t tcp_sender_endpoint_init(endpoint_t *const endpoint) {
  if (endpoint == NULL) {
    return RC_NULL_PARAM;
  }

  try {
    boost::asio::io_context ctx;
    boost::asio::ip::tcp::resolver resolver(ctx);
    boost::asio::ip::tcp::resolver::query query(endpoint->host, std::to_string(endpoint->port));
    boost::asio::ip::tcp::endpoint destination = *resolver.resolve(query);
    strcpy(endpoint->ip, destination.address().to_string().c_str());
  } catch (...) {
    return RC_NEIGHBOR_FAILED_ENDPOINT_INIT;
  }

  return RC_OK;
}

retcode_t tcp_sender_endpoint_destroy(endpoint_t *const endpoint) {
  if (endpoint == NULL) {
    return RC_OK;
  }

  boost::asio::ip::tcp::endpoint *destination =
      reinterpret_cast<boost::asio::ip::tcp::endpoint *>(endpoint->opaque_inetaddr);
  delete destination;
  endpoint->opaque_inetaddr = NULL;

  return RC_OK;
}

bool tcp_send(endpoint_t *const endpoint, iota_packet_t const *const packet) {
  if (endpoint == NULL) {
    return false;
  } else if (endpoint->opaque_inetaddr == NULL) {
    return true;
  }

  try {
    char crc[CRC_SIZE + 1];
    auto socket = reinterpret_cast<boost::asio::ip::tcp::socket *>(endpoint->opaque_inetaddr);
    boost::system::error_code ignored_error;
    boost::crc_32_type result;

    result.process_bytes(packet->content, PACKET_SIZE);
    snprintf(crc, CRC_SIZE + 1, "%0*x", CRC_SIZE, result.checksum());
    boost::asio::write(*socket, boost::asio::buffer(packet->content), ignored_error);
    boost::asio::write(*socket, boost::asio::buffer(crc, CRC_SIZE), ignored_error);
  } catch (...) {
    return false;
  }
  return true;
}
