/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/receiver.h"
#include "common/network/logger.h"

bool receiver_init(receiver_state_t *const state, node_t *const node,
                   uint16_t tcp_port, uint16_t udp_port) {
  if (state == NULL || node == NULL) {
    return false;
  }
  state->running = false;
  state->tcp_service.port = tcp_port;
  state->tcp_service.protocol = PROTOCOL_TCP;
  state->tcp_service.context = NULL;
  state->udp_service.port = udp_port;
  state->udp_service.protocol = PROTOCOL_UDP;
  state->udp_service.context = NULL;
  state->node = node;
  return true;
}

bool receiver_start(receiver_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  state->running = true;
  if (state->tcp_service.port != 0) {
    log_info("Spawning TCP receiver thread with port %d",
             state->tcp_service.port);
    if (thread_handle_create(&state->tcp_service.thread,
                             (thread_routine_t)receiver_service_start,
                             &state->tcp_service) != 0) {
      log_fatal("Spawning TCP receiver thread failed", state->tcp_service.port);
      return false;
    }
  }
  if (state->udp_service.port != 0) {
    log_info("Spawning UDP receiver thread with port %d",
             state->udp_service.port);
    if (thread_handle_create(&state->udp_service.thread,
                             (thread_routine_t)receiver_service_start,
                             &state->udp_service) != 0) {
      log_info("Spawning UDP receiver thread failed", state->udp_service.port);
      return false;
    }
  }
  return true;
}

bool receiver_stop(receiver_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  state->running = false;
  log_info("Shutting down TCP receiver thread");
  receiver_service_stop(&state->tcp_service);
  if (thread_handle_join(state->tcp_service.thread, NULL) != 0) {
    return false;
  }
  log_info("Shutting down UDP receiver thread");
  receiver_service_stop(&state->udp_service);
  if (thread_handle_join(state->udp_service.thread, NULL) != 0) {
    return false;
  }
  return true;
}
