/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/logger_helper.h"
#include "common/network/services/tcp_receiver.hpp"
#include "common/network/services/udp_receiver.hpp"

static char const receiver_service_logger[] = "receiver_service";

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
  logger_helper_init(receiver_service_logger, LOGGER_DEBUG, true);
  try {
    boost::asio::io_context ctx;
    service->context = &ctx;
    if (service->protocol == PROTOCOL_TCP) {
      log_info(receiver_service_logger,
               "Starting TCP receiver service on port %d\n", service->port);
      TcpReceiverService tcpService(service, ctx, service->port);
      ctx.run();
    } else if (service->protocol == PROTOCOL_UDP) {
      log_info(receiver_service_logger,
               "Starting UDP receiver service on port %d\n", service->port);
      UdpReceiverService udpService(service, ctx, service->port);
      ctx.run();
    } else {
      log_error(receiver_service_logger,
                "Starting receiver service failed: unknown protocol\n");
      return false;
    }
  } catch (std::exception const& e) {
    log_error(receiver_service_logger, "Starting receiver service failed: %s\n",
              e.what());
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
      log_error(receiver_service_logger,
                "Stopping receiver service failed: invalid context\n");
      return false;
    }
    ctx->stop();
  } catch (std::exception const& e) {
    log_error(receiver_service_logger, "Stopping receiver service failed: %s\n",
              e.what());
    return false;
  }
  logger_helper_destroy(receiver_service_logger);
  return true;
}
