/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "processor.h"
#include "common/network/logger.h"

void processor_on_next(processor_state_t *const state,
                       trit_array_p const hash) {
  state->queue->vtable->push(state->queue, hash);
}

void *processor_routine(processor_state_t *const state) {
  trit_array_p hash;

  log_info("Spawning processor thread");
  state->running = true;
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &hash) ==
        CONCURRENT_QUEUE_SUCCESS) {
      // TODO(thibault@iota.org) actual processing of the hash
    }
  }
  log_info("Shutting down processor thread");
  return NULL;
}
