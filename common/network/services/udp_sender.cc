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
  if (endpoint == NULL) {
    return false;
  }
  // auto sock = reinterpret_cast<boost::asio::ip::tcp::socket
  // *>(opaque_inetaddr); try {
  //   boost::asio::write(*sock, boost::asio::buffer("Broadcast\n", 10));
  // } catch (std::exception const &e) {
  //   log_error("TCP write failed:  %s", e.what());
  //   return false;
  // }
  return true;
}
