/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/services/receiver.h"
#include "common/network/logger.h"

bool receiver_packet_handler(receiver_state_t *const state,
                             iota_packet_t *const packet) {
  if (packet->length != TRANSACTION_PACKET_SIZE) {
    return false;
  }
  log_debug("%s packet received from %s:%d",
            (packet->source.protocol == ENDPOINT_PROTOCOL_TCP ? "TCP" : "UDP"),
            packet->source.host, packet->source.port);
  // TODO(thibault) submit packet for pre-processing
  return true;
}

static void *receiver_routine(receiver_state_t *const state) {
  if (state == NULL) {
    return NULL;
  }
  receiver_service_start(state);
  return NULL;
}

bool receiver_init(receiver_state_t *const state, node_t *const node, uint16_t tcp_port,
                    uint16_t udp_port) {
  if (state == NULL || node == NULL) {
    return false;
  }
  state->running = false;
  state->opaque_network = NULL;
  state->tcp_port = tcp_port;
  state->udp_port = udp_port;
  state->node = node;
  return true;
}

bool receiver_start(receiver_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  log_info("Spawning receiver thread");
  state->running = true;
  if (thread_handle_create(&state->thread, (thread_routine_t)receiver_routine,
                           state) != 0) {
    return false;
  }
  return true;
}

bool receiver_stop(receiver_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  log_info("Shutting down receiver thread");
  state->running = false;
  receiver_service_stop(state);
  if (thread_handle_join(state->thread, NULL) != 0) {
    return false;
  }
  return true;
}
