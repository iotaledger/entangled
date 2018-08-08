/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/responder.h"
#include "utils/logger_helper.h"

static char const responder_component_logger[] = "responder_component";

static void *responder_routine(responder_state_t *const state) {
  hash_request_t request;

  if (state == NULL) {
    return NULL;
  }
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &request) ==
        CONCURRENT_QUEUE_SUCCESS) {
      // TODO(thibault) respond to request
    }
  }
  return NULL;
}

bool responder_init(responder_state_t *const state, node_t *const node) {
  if (state == NULL || node == NULL) {
    return false;
  }
  logger_helper_init(responder_component_logger, LOGGER_DEBUG, true);
  state->running = false;
  if (INIT_CONCURRENT_QUEUE_OF(hash_request_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_critical(responder_component_logger,
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
  log_info(responder_component_logger, "Spawning responder thread\n");
  state->running = true;
  if (thread_handle_create(&state->thread, (thread_routine_t)responder_routine,
                           state) != 0) {
    log_critical(responder_component_logger,
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
  if (state->queue->vtable->push(state->queue,
                                 (hash_request_t){hash, *neighbor}) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_warning(responder_component_logger,
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
  log_info(responder_component_logger, "Shutting down responder thread\n");
  state->running = false;
  if (thread_handle_join(state->thread, NULL) != 0) {
    log_error(responder_component_logger,
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
  if (DESTROY_CONCURRENT_QUEUE_OF(hash_request_t, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_error(responder_component_logger,
              "Destroying responder queue failed\n");
    ret = false;
  }
  logger_helper_destroy(responder_component_logger);
  return ret;
}
