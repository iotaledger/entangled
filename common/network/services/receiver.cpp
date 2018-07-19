/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/services/receiver.h"
#include "common/network/logger.h"
#include "common/network/services/udp_receiver.hpp"

bool receiver_service_start(receiver_state_t* const state) {
  if (state == NULL) {
    return false;
  }
  try {
    boost::asio::io_context ctx;
    state->opaque_network = &ctx;
    // TODO(thibault) port as parameter
    UdpReceiverService udpService(ctx, 4242);
    ctx.run();
  } catch (std::exception const& e) {
    log_error("Receiver service failed to start: %s", e.what());
    return false;
  }
  return true;
}

bool receiver_service_stop(receiver_state_t* const state) {
  if (state == NULL) {
    return false;
  }
  try {
    auto ctx =
        reinterpret_cast<boost::asio::io_context*>(state->opaque_network);
    if (ctx == NULL) {
      log_error(
          "Receiver service failed to stop: invalid network configuration");
      return false;
    }
    ctx->stop();
  } catch (std::exception const& e) {
    log_error("Receiver service failed to stop: %s", e.what());
    return false;
  }
  return true;
}
