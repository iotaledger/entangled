/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "receiver.h"
#include "common/network/logger.h"

void *receiver_routine(receiver_state_t *const state) {
  log_info("Spawning receiver thread");
  state->running = true;
  while (state->running) {
    // TODO(thibault@iota.org) actual receiving
  }
  log_info("Shutting down receiver thread");
  return NULL;
}
