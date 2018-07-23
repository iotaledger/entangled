/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/services/receiver.h"
#include "common/network/logger.h"
#include "common/network/services/tcp_receiver.hpp"
#include "common/network/services/udp_receiver.hpp"

void receiver_service_prepare_packet(iota_packet_t* const packet,
                                     size_t const length,
                                     char const* const host,
                                     uint16_t const port,
                                     protocol_type_t const protocol) {
  // TODO(thibault) check size packet
  // TODO(thibault) check size host
  strcpy(packet->source.host, host);
  packet->source.port = port;
  packet->source.protocol = protocol;
  packet->content[length] = '\0';
}

bool receiver_service_start(receiver_state_t* const state) {
  if (state == NULL) {
    return false;
  }
  try {
    boost::asio::io_context ctx;
    state->opaque_network = &ctx;
    TcpReceiverService tcpService(state, ctx, state->tcp_port);
    UdpReceiverService udpService(state, ctx, state->udp_port);
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
      log_error("Receiver service failed to stop: invalid service");
      return false;
    }
    ctx->stop();
  } catch (std::exception const& e) {
    log_error("Receiver service failed to stop: %s", e.what());
    return false;
  }
  return true;
}
