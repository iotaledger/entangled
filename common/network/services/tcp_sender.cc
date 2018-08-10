/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/asio.hpp>

#include "common/network/services/tcp_sender.hpp"
#include "utils/logger_helper.h"

bool tcp_send(endpoint_t *const endpoint, trit_array_p const hash) {
  if (endpoint == NULL || endpoint->opaque_inetaddr == NULL) {
    return false;
  }
  size_t trytes_num = num_trytes_for_trits(hash->num_trits);
  tryte_t trytes[trytes_num];
  if (flex_trit_to_tryte(trytes, trytes_num, hash->trits, hash->num_trits,
                         hash->num_trits) == 0) {
    return false;
  }
  try {
    auto socket = reinterpret_cast<boost::asio::ip::tcp::socket *>(
        endpoint->opaque_inetaddr);
    boost::asio::async_write(
        *socket, boost::asio::buffer(trytes, trytes_num),
        [](const boost::system::error_code &, std::size_t) {});
  } catch (std::exception const &e) {
    log_error("Sending packet to tcp://%s:%d failed: %s", endpoint->host,
              endpoint->port, e.what());
    return false;
  }
  return true;
}
