/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/requester.h"
#include "ciri/core.h"
#include "common/storage/sql/defs.h"
#include "common/storage/storage.h"
#include "utils/containers/lists/concurrent_list_trit_array.h"
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
  if (CL_INIT(trit_array_p, state->list, trit_array_cmp) != CL_SUCCESS) {
    log_critical(REQUESTER_COMPONENT_LOGGER_ID,
                 "Initializing requester list failed\n");
    return false;
  }
  state->node = node;
  return true;
}

size_t requester_size(requester_state_t *const state) {
  if (state == NULL) {
    return 0;
  }
  return CL_SIZE(state->list);
}

bool requester_clear_request(requester_state_t *const state,
                             trit_array_p const hash) {
  if (state == NULL) {
    return false;
  }
  if (hash == NULL) {
    return false;
  }
  if (CL_REMOVE(state->list, hash) != CL_SUCCESS) {
    return false;
  }
  return true;
}

bool requester_is_full(requester_state_t *const state) {
  if (state == NULL) {
    return true;
  }
  return CL_SIZE(state->list) >= MAX_TX_REQ_NBR;
}

bool request_transaction(requester_state_t *const state,
                         trit_array_p const hash) {
  bool exists = false;

  if (state == NULL) {
    return false;
  }
  if (hash == NULL) {
    return false;
  }
  // TODO(thibault) check null hash
  if (iota_stor_exist(&state->node->core->db_conn, COL_HASH, hash, &exists)) {
    return false;
  }
  if (exists) {
    return false;
  }
  if (CL_CONTAINS(state->list, hash) == false) {
    if (requester_is_full(state) == false) {
      if (CL_PUSH_BACK(state->list, hash) != CL_SUCCESS) {
        log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                    "Adding new transaction request to the list failed\n");
        return false;
      }
    } else {
      log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                  "Transactions requests list is full\n");
    }
  }
  return true;
}

bool get_transaction_to_request(requester_state_t *const state,
                                trit_array_p *hash) {
  concurrent_list_node_trit_array_p *iter = NULL;
  bool exists = false;

  if (state == NULL) {
    return false;
  }
  if (hash == NULL) {
    return false;
  }
  iter = state->list->front;
  while (iter) {
    *hash = iter->data;
    iter = iter->next;
    if (iota_stor_exist(&state->node->core->db_conn, COL_HASH, *hash,
                        &exists)) {
      return false;
    }
    if (exists) {
      requester_clear_request(state, *hash);
    } else {
      break;
    }
  }
  if (iter == NULL) {
    *hash = NULL;
  }
  return true;
}

bool requester_destroy(requester_state_t *const state) {
  bool ret = true;

  if (state == NULL) {
    return false;
  }
  if (CL_DESTROY(trit_array_p, state->list) != CL_SUCCESS) {
    ret = false;
  }
  logger_helper_destroy(REQUESTER_COMPONENT_LOGGER_ID);
  return ret;
}
