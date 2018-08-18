/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/responder.h"
#include "utils/logger_helper.h"

#define RESPONDER_COMPONENT_LOGGER_ID "responder_component"

static void *responder_routine(responder_state_t *const state) {
  hash_request_t request;

  if (state == NULL) {
    return NULL;
  }
  while (state->running) {
    if (CQ_POP(state->queue, &request) == CONCURRENT_QUEUE_SUCCESS) {
      log_debug(RESPONDER_COMPONENT_LOGGER_ID, "Responding to request\n");
      // TODO(thibault) respond to request
    }
  }
  return NULL;
}

bool responder_init(responder_state_t *const state, node_t *const node) {
  if (state == NULL || node == NULL) {
    return false;
  }
  logger_helper_init(RESPONDER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  state->running = false;
  if (INIT_CONCURRENT_QUEUE_OF(hash_request_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_critical(RESPONDER_COMPONENT_LOGGER_ID,
                 "Initializing responder queue failed\n");
    return false;
  }
  state->node = node;
  return true;
}

bool responder_start(responder_state_t *const state) {
  if (state == NULL) {
    return false;
  }
  log_info(RESPONDER_COMPONENT_LOGGER_ID, "Spawning responder thread\n");
  state->running = true;
  if (thread_handle_create(&state->thread, (thread_routine_t)responder_routine,
                           state) != 0) {
    log_critical(RESPONDER_COMPONENT_LOGGER_ID,
                 "Spawning responder thread failed\n");
    return false;
  }
  return true;
}

bool responder_on_next(responder_state_t *const state, trit_array_p const hash,
                       neighbor_t *const neighbor) {
  if (state == NULL) {
    return false;
  }
  if (CQ_PUSH(state->queue, ((hash_request_t){hash, *neighbor})) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_warning(RESPONDER_COMPONENT_LOGGER_ID,
                "Pushing to responder queue failed\n");
    return false;
  }
  return true;
}

bool responder_stop(responder_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  log_info(RESPONDER_COMPONENT_LOGGER_ID, "Shutting down responder thread\n");
  state->running = false;
  if (thread_handle_join(state->thread, NULL) != 0) {
    log_error(RESPONDER_COMPONENT_LOGGER_ID,
              "Shutting down responder thread failed\n");
    ret = false;
  }
  return ret;
}

bool responder_destroy(responder_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  if (state->running) {
    return false;
  }
  if (DESTROY_CONCURRENT_QUEUE_OF(hash_request_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_error(RESPONDER_COMPONENT_LOGGER_ID,
              "Destroying responder queue failed\n");
    ret = false;
  }
  logger_helper_destroy(RESPONDER_COMPONENT_LOGGER_ID);
  return ret;
}
