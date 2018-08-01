/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/asio.hpp>

#include "common/network/services/tcp_sender.hpp"

bool tcp_send(void *opaque_inetaddr) {
  if (opaque_inetaddr == NULL) {
    return false;
  }
  auto sock = reinterpret_cast<boost::asio::ip::tcp::socket *>(opaque_inetaddr);
  boost::asio::write(*sock, boost::asio::buffer("Broadcast\n", 10));
  return true;
}
