/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "broadcaster.h"
#include "common/network/logger.h"

void broadcaster_on_next(broadcaster_state_t *const state,
                         trit_array_p const hash) {
  state->queue->vtable->push(state->queue, hash);
}

void *broadcaster_routine(broadcaster_state_t *const state) {
  trit_array_p hash;

  log_info("Spawning broadcaster thread");
  state->running = true;
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &hash) ==
        CONCURRENT_QUEUE_SUCCESS) {
      // for (size_t i = 0; neighbors[i]; ++i) {
      // TODO(thibault@iota.org) actual broadcasting of the hash
      // }
    }
  }
  log_info("Shutting down broadcaster thread");
  return NULL;
}
