/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/broadcaster.h"
#include "common/network/logger.h"

static void *broadcaster_routine(broadcaster_state_t *const state) {
  trit_array_p hash;

  if (state == NULL) return NULL;
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &hash) ==
        CONCURRENT_QUEUE_SUCCESS) {
      // TODO(thibault) broadcast the hash
    }
  }
  return NULL;
}

bool broadcaster_start(broadcaster_state_t *const state) {
  if (state == NULL) return false;
  if (INIT_CONCURRENT_QUEUE_OF(trit_array_p, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS)
    return false;
  log_info("Spawning broadcaster thread");
  state->running = true;
  thread_handle_create(&state->thread, (thread_routine_t)broadcaster_routine,
                       state);
  return true;
}

bool broadcaster_on_next(broadcaster_state_t *const state,
                         trit_array_p const hash) {
  if (state == NULL) return false;
  return state->queue->vtable->push(state->queue, hash) ==
         CONCURRENT_QUEUE_SUCCESS;
}

bool broadcaster_stop(broadcaster_state_t *const state) {
  if (state == NULL) return false;
  log_info("Shutting down broadcaster thread");
  state->running = false;
  thread_handle_join(state->thread, NULL);
  if (DESTROY_CONCURRENT_QUEUE_OF(trit_array_p, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS)
    return false;
  return true;
}
