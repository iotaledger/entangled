/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/receiver.h"
#include "common/network/logger.h"
#include "common/network/services/receiver.h"

void packet_handler(receiver_state_t *const state,
                    iota_packet_t *const packet) {
  // TODO(thibault) discard if bad size
  log_debug("Packet received from %s:%d", packet->source.host,
            packet->source.port);
  // TODO(thibault) submit packet for pre-processing
}

static void *receiver_routine(receiver_state_t *const state) {
  if (state == NULL) {
    return NULL;
  }
  receiver_service_start(state);
  return NULL;
}

bool receiver_start(receiver_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  state->opaque_network = NULL;
  log_info("Spawning receiver thread");
  state->running = true;
  thread_handle_create(&state->thread, (thread_routine_t)receiver_routine,
                       state);
  return true;
}

bool receiver_stop(receiver_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  log_info("Shutting down receiver thread");
  state->running = false;
  receiver_service_stop(state);
  thread_handle_join(state->thread, NULL);
  return true;
}
