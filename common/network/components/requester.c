/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/requester.h"
#include "utils/logger_helper.h"

// TODO(thibault) configuration variable
#define MAX_TX_REQ_QUEUE_SIZE 10000
#define REQUESTER_COMPONENT_LOGGER_ID "requester_component"

bool requester_init(requester_state_t *const state, node_t *const node) {
  if (state == NULL) {
    return false;
  }
  if (node == NULL) {
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

size_t transactions_to_request_number(requester_state_t *const state) {
  if (state == NULL) {
    return 0;
  }
  return state->queue->vtable->size(state->queue);
}

bool clear_transaction_request(trit_array_p const hash) {
  if (hash == NULL) {
    return false;
  }
  // TODO(thibault) remove from set/queue
  return true;
}

bool transactions_to_request_is_full(requester_state_t *const state) {
  if (state == NULL) {
    return true;
  }
  return state->queue->vtable->size(state->queue) >= MAX_TX_REQ_QUEUE_SIZE;
}

bool request_transaction(requester_state_t *const state,
                         trit_array_p const hash) {
  if (state == NULL) {
    return false;
  }
  if (hash == NULL) {
    return false;
  }
  if (!transactions_to_request_is_full(state)) {
    if (state->queue->vtable->push(state->queue, hash) !=
        CONCURRENT_QUEUE_SUCCESS) {
      log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                  "Pushing to requester queue failed\n");
      return false;
    }
  }
  return true;
}


bool get_transaction_to_request(requester_state_t *const state,
                                trit_array_p *hash) {
  if (state == NULL) {
    return false;
  }
  if (state->queue->vtable->pop(state->queue, hash) !=
      CONCURRENT_QUEUE_SUCCESS) {
    log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                "Popping from requester queue failed\n");
    return false;
  }
  return true;
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
