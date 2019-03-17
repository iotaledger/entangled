/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/services/receiver.h"
#include "gossip/services/tcp_receiver.hpp"
#include "gossip/services/udp_receiver.hpp"
#include "utils/logger_helper.h"

#define RECEIVER_SERVICE_LOGGER_ID "receiver_service"

static logger_id_t logger_id;

bool receiver_service_start(receiver_service_t* const service) {
  if (service == NULL) {
    return false;
  }
  logger_id = logger_helper_enable(RECEIVER_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);
  try {
    boost::asio::io_context ctx;
    service->context = &ctx;
    if (service->protocol == PROTOCOL_TCP) {
      log_info(logger_id, "Starting TCP receiver service on port %d\n", service->port);
      TcpReceiverService tcpService(service, ctx, service->port);
      ctx.run();
    } else if (service->protocol == PROTOCOL_UDP) {
      log_info(logger_id, "Starting UDP receiver service on port %d\n", service->port);
      UdpReceiverService udpService(service, ctx, service->port);
      ctx.run();
    } else {
      log_error(logger_id, "Starting receiver service failed: unknown protocol\n");
      return false;
    }
  } catch (std::exception const& e) {
    log_error(logger_id, "Starting receiver service failed: %s\n", e.what());
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
      log_error(logger_id, "Stopping receiver service failed: invalid context\n");
      return false;
    }
    ctx->stop();
  } catch (std::exception const& e) {
    log_error(logger_id, "Stopping receiver service failed: %s\n", e.what());
    return false;
  }
  logger_helper_release(logger_id);
  return true;
}
