/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/asio.hpp>

#include "common/network/logger.h"
#include "common/network/services/udp_sender.hpp"

bool udp_send(endpoint_t *const endpoint, trit_array_p const hash) {
  boost::asio::ip::address addr;

  if (endpoint == NULL || endpoint->opaque_inetaddr == NULL) {
    return false;
  }
  size_t trytes_num = num_trytes_for_trits(hash->num_trits);
  trit_t trytes[trytes_num];
  if (flex_trit_to_tryte(trytes, trytes_num, hash->trits, hash->num_trits,
                         hash->num_trits) == 0) {
    return false;
  }
  try {
    auto socket = reinterpret_cast<boost::asio::ip::udp::socket *>(
        endpoint->opaque_inetaddr);
    addr.from_string(endpoint->host);
    boost::asio::ip::udp::endpoint sender_endpoint(addr, endpoint->port);
    socket->send_to(boost::asio::buffer(trytes, trytes_num), sender_endpoint);
  } catch (std::exception const &e) {
    log_error("Sending packet to udp://%s:%d failed: %s", endpoint->host,
              endpoint->port, e.what());
    return false;
  }
  return true;
}
