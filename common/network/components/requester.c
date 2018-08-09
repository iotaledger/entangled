/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/requester.h"
#include "utils/logger_helper.h"

#define REQUESTER_COMPONENT_LOGGER_ID "requester_component"

bool requester_init(requester_state_t *const state, node_t *const node) {
  if (state == NULL || node == NULL) {
    return false;
  }
  logger_helper_init(REQUESTER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  if (INIT_CONCURRENT_QUEUE_OF(trit_array_p, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_critical(REQUESTER_COMPONENT_LOGGER_ID,
                 "Initializing requester queue failed\n");
    return false;
  }
  state->node = node;
  return true;
}

bool request_transaction(requester_state_t *const state,
                         trit_array_p const hash) {
  if (state == NULL) {
    return false;
  }
  if (state->queue->vtable->push(state->queue, hash) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                "Pushing to requester queue failed\n");
    return false;
  }
  return true;
}

trit_array_p get_transaction_to_request(requester_state_t *const state) {
  trit_array_p hash;

  if (state == NULL) {
    return NULL;
  }
  if (state->queue->vtable->pop(state->queue, &hash) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                "Popping from requester queue failed\n");
    return NULL;
  }
  return hash;
}

bool requester_destroy(requester_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  if (DESTROY_CONCURRENT_QUEUE_OF(trit_array_p, state->queue) !=
      CONCURRENT_QUEUE_SUCCESS) {
    ret = false;
  }
  logger_helper_destroy(REQUESTER_COMPONENT_LOGGER_ID);
  return ret;
}
