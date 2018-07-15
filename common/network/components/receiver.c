/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "receiver.h"
#include "common/network/logger.h"

static void *receiver_routine(receiver_state_t *const state) {
  if (state == NULL) return NULL;
  while (state->running) {
    // TODO(thibault) receive
  }
  return NULL;
}

bool receiver_init(receiver_state_t *const state) {
  if (state == NULL) return false;
  state->running = true;
  log_info("Spawning receiver thread");
  // TODO(thibault) spawn thread
  return true;
}

bool receiver_stop(receiver_state_t *const state) {
  if (state == NULL) return false;
  state->running = false;
  log_info("Shutting down receiver thread");
  // TODO(thibault) join thread
  return true;
}
