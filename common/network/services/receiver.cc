/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/logger.h"
#include "common/network/services/tcp_receiver.hpp"
#include "common/network/services/udp_receiver.hpp"

void receiver_service_prepare_packet(iota_packet_t* const packet,
                                     size_t const length,
                                     char const* const host,
                                     uint16_t const port,
                                     protocol_type_t const protocol) {
  packet->length = length;
  strcpy(packet->source.host, host);
  packet->source.port = port;
  packet->source.protocol = protocol;
  packet->content[length] = '\0';
}

bool receiver_service_start(receiver_service_t* const service) {
  if (service == NULL) {
    return false;
  }
  try {
    boost::asio::io_context ctx;
    service->context = &ctx;
    if (service->protocol == PROTOCOL_TCP) {
      TcpReceiverService tcpService(service, ctx, service->port);
      ctx.run();
    } else if (service->protocol == PROTOCOL_UDP) {
      UdpReceiverService udpService(service, ctx, service->port);
      ctx.run();
    } else {
      log_error("Starting receiver service failed: unknown protocol");
      return false;
    }
  } catch (std::exception const& e) {
    log_error("Starting receiver service failed: %s", e.what());
    return false;
  }
  return true;
}

bool receiver_service_stop(receiver_service_t* const service) {
  if (service == NULL) {
    return false;
  }
  try {
    auto ctx = reinterpret_cast<boost::asio::io_context*>(service->context);
    if (ctx == NULL) {
      log_error("Receiver service failed to stop: invalid context");
      return false;
    }
    ctx->stop();
  } catch (std::exception const& e) {
    log_error("Receiver service failed to stop: %s", e.what());
    return false;
  }
  return true;
}
