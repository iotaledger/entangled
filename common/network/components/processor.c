/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "processor.h"
#include "common/network/logger.h"

static void *processor_routine(processor_state_t *const state) {
  trit_array_p hash;

  if (state == NULL) return NULL;
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &hash) ==
        CONCURRENT_QUEUE_SUCCESS) {
      // TODO(thibault) process the hash
    }
  }
  return NULL;
}

bool processor_start(processor_state_t *const state) {
  if (state == NULL) return false;
  if (INIT_CONCURRENT_QUEUE_OF(trit_array_p, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS)
    return false;
  log_info("Spawning processor thread");
  state->running = true;
  thread_handle_create(&state->thread, (thread_routine_t)processor_routine,
                       state);
  return true;
}

bool processor_on_next(processor_state_t *const state,
                       trit_array_p const hash) {
  if (state == NULL) return false;
  return state->queue->vtable->push(state->queue, hash) ==
         CONCURRENT_QUEUE_SUCCESS;
}

bool processor_stop(processor_state_t *const state) {
  if (state == NULL) return false;
  if (DESTROY_CONCURRENT_QUEUE_OF(trit_array_p, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS)
    return false;
  log_info("Shutting down processor thread");
  state->running = false;
  thread_handle_join(state->thread, NULL);
  return true;
}
