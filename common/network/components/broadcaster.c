/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "broadcaster.h"

void broadcaster_on_next(broadcaster_state_t *const state,
                         trit_array_p const hash) {
  state->queue->vtable->push(state->queue, hash);
}

void *broadcaster_routine(broadcaster_state_t *const state) {
  trit_array_p hash;

  // TODO(thibault@iota.org) logger
  printf("Spawning broadcaster thread\n");
  state->running = true;
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &hash) ==
        CONCURRENT_QUEUE_SUCCESS) {
      // for (size_t i = 0; neighbors[i]; ++i) {
      // TODO(thibault@iota.org) actual broadcasting of the hash
      // }
    }
  }
  printf("Shutting down broadcaster thread\n");
  return NULL;
}
