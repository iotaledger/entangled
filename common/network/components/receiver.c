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

bool receiver_start(receiver_state_t *const state) {
  if (state == NULL) {
    return false;
  }
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
  thread_handle_join(state->thread, NULL);
  return true;
}
