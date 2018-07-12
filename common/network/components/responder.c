/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "responder.h"
#include "common/network/logger.h"

void responder_on_next(responder_state_t *const state, trit_array_p const hash,
                       neighbor_t *const neighbor) {
  state->queue->vtable->push(state->queue, (hash_request_t){hash, *neighbor});
}

void *responder_routine(responder_state_t *const state) {
  hash_request_t request;

  log_info("Spawning responder thread");
  state->running = true;
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &request) ==
        CONCURRENT_QUEUE_SUCCESS) {
      // TODO(thibault@iota.org) actual responding to the request
    }
  }
  log_info("Shutting down responder thread");
  return NULL;
}
