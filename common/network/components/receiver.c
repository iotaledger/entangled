/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/receiver.h"
#include "common/network/logger.h"

static void *receiver_routine(receiver_state_t *const state) {
  if (state == NULL) {
    return NULL;
  }
  while (state->running) {
    // TODO(thibault) receive
  }
  return NULL;
}

bool receiver_init(receiver_state_t *const state, node_t *const node) {
  if (state == NULL || node == NULL) {
    return false;
  }
  state->running = false;
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
  if (thread_handle_join(state->thread, NULL) != 0) {
    return false;
  }
  return true;
}
