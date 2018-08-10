/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/requester.h"
#include "utils/logger_helper.h"

// TODO(thibault) configuration variable
#define MAX_TX_REQ_NBR 10000
#define REQUESTER_COMPONENT_LOGGER_ID "requester_component"

bool requester_init(requester_state_t *const state, node_t *const node) {
  if (state == NULL) {
    return false;
  }
  if (node == NULL) {
    return false;
  }
  logger_helper_init(REQUESTER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  if (INIT_CONCURRENT_LIST_OF(trit_array_p, state->list, trit_array_cmp) !=
      CONCURRENT_LIST_SUCCESS) {
    log_critical(REQUESTER_COMPONENT_LOGGER_ID,
                 "Initializing requester list failed\n");
    return false;
  }
  state->node = node;
  return true;
}

size_t transactions_to_request_number(requester_state_t *const state) {
  if (state == NULL) {
    return 0;
  }
  return state->list->vtable->size(state->list);
}

bool clear_transaction_request(requester_state_t *const state,
                               trit_array_p const hash) {
  if (state == NULL) {
    return false;
  }
  if (hash == NULL) {
    return false;
  }
  if (state->list->vtable->remove(state->list, hash) !=
      CONCURRENT_LIST_SUCCESS) {
    return false;
  }
  return true;
}

bool transactions_to_request_is_full(requester_state_t *const state) {
  if (state == NULL) {
    return true;
  }
  return state->list->vtable->size(state->list) >= MAX_TX_REQ_NBR;
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
    if (state->list->vtable->push_back(state->list, hash) !=
        CONCURRENT_LIST_SUCCESS) {
      log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                  "Pushing to requester list failed\n");
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
    return false;
  }
  return true;
}

bool requester_destroy(requester_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  if (DESTROY_CONCURRENT_LIST_OF(trit_array_p, state->list) !=
      CONCURRENT_QUEUE_SUCCESS) {
    ret = false;
  }
  logger_helper_destroy(REQUESTER_COMPONENT_LOGGER_ID);
  return ret;
}
