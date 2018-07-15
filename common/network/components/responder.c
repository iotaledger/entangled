/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "responder.h"
#include "common/network/logger.h"

static void *responder_routine(responder_state_t *const state) {
  hash_request_t request;

  if (state == NULL) return NULL;
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &request) ==
        CONCURRENT_QUEUE_SUCCESS) {
      // TODO(thibault) respond to request
    }
  }
  return NULL;
}

bool responder_start(responder_state_t *const state) {
  if (state == NULL) return false;
  if (INIT_CONCURRENT_QUEUE_OF(hash_request_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS)
    return false;
  state->running = true;
  log_info("Spawning responder thread");
  // TODO(thibault) spawn thread
  return true;
}

bool responder_on_next(responder_state_t *const state, trit_array_p const hash,
                       neighbor_t *const neighbor) {
  if (state == NULL) return false;
  return state->queue->vtable->push(state->queue,
                                    (hash_request_t){hash, *neighbor}) ==
         CONCURRENT_QUEUE_SUCCESS;
}

bool responder_stop(responder_state_t *const state) {
  if (state == NULL) return false;
  state->running = false;
  if (DESTROY_CONCURRENT_QUEUE_OF(hash_request_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS)
    return false;
  log_info("Shutting down responder thread");
  // TODO(thibault) join thread
  return true;
}
