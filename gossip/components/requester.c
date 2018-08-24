/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/components/requester.h"
#include "ciri/core.h"
#include "common/storage/sql/defs.h"
#include "common/storage/storage.h"
#include "utils/containers/lists/concurrent_list_trit_array.h"
#include "utils/logger_helper.h"

// TODO(thibault) configuration variable
#define MAX_TX_REQ_NBR 10000
#define REQUESTER_COMPONENT_LOGGER_ID "requester_component"

retcode_t requester_init(requester_state_t *const state, node_t *const node) {
  if (state == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }
  if (node == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_NODE;
  }

  logger_helper_init(REQUESTER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  memset(state, 0, sizeof(requester_state_t));
  state->node = node;

  log_debug(REQUESTER_COMPONENT_LOGGER_ID, "Initializing requester list\n");
  if (CL_INIT(trit_array_p, state->list, trit_array_cmp) != CL_SUCCESS) {
    log_critical(REQUESTER_COMPONENT_LOGGER_ID,
                 "Initializing requester list failed\n");
    return RC_REQUESTER_COMPONENT_FAILED_INIT_LIST;
  }
  return RC_OK;
}

size_t requester_size(requester_state_t *const state) {
  if (state == NULL) {
    return 0;
  }

  return CL_SIZE(state->list);
}

retcode_t requester_clear_request(requester_state_t *const state,
                                  trit_array_p const hash) {
  if (state == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }
  if (hash == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_HASH;
  }

  if (CL_REMOVE(state->list, hash) != CL_SUCCESS) {
    log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                "Clearing request from list failed\n");
    return RC_REQUESTER_COMPONENT_FAILED_REMOVE_LIST;
  }
  return RC_OK;
}

bool requester_is_full(requester_state_t *const state) {
  if (state == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  return CL_SIZE(state->list) >= MAX_TX_REQ_NBR;
}

retcode_t request_transaction(requester_state_t *const state,
                              trit_array_p const hash) {
  retcode_t ret = RC_OK;
  bool exists = false;

  if (state == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }
  if (hash == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_HASH;
  }

  // TODO(thibault) check null hash
  if ((ret = iota_stor_exist(&state->node->core->db_conn, COL_HASH, hash,
                             &exists))) {
    return ret;
  }
  if (exists) {
    return RC_OK;
  }
  if (CL_CONTAINS(state->list, hash) == false) {
    if (requester_is_full(state) == false) {
      if (CL_PUSH_BACK(state->list, hash) != CL_SUCCESS) {
        log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                    "Adding new transaction request to the list failed\n");
        return RC_REQUESTER_COMPONENT_FAILED_ADD_LIST;
      }
    } else {
      log_warning(REQUESTER_COMPONENT_LOGGER_ID,
                  "Transactions requests list is full\n");
    }
  }
  return RC_OK;
}

retcode_t get_transaction_to_request(requester_state_t *const state,
                                     trit_array_p *hash) {
  retcode_t ret = RC_OK;
  concurrent_list_node_trit_array_p *iter = NULL;
  bool exists = false;

  if (state == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }
  if (hash == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_HASH;
  }

  iter = state->list->front;
  while (iter) {
    *hash = iter->data;
    iter = iter->next;
    if ((ret = iota_stor_exist(&state->node->core->db_conn, COL_HASH, *hash,
                               &exists))) {
      return ret;
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
  return RC_OK;
}

retcode_t requester_destroy(requester_state_t *const state) {
  retcode_t ret = RC_OK;

  if (state == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  log_debug(REQUESTER_COMPONENT_LOGGER_ID, "Destroying requester list\n");
  if (CL_DESTROY(trit_array_p, state->list) != CL_SUCCESS) {
    log_error(REQUESTER_COMPONENT_LOGGER_ID,
              "Destroying requester list failed\n");
    ret = RC_REQUESTER_COMPONENT_FAILED_DESTROY_LIST;
  }
  logger_helper_destroy(REQUESTER_COMPONENT_LOGGER_ID);
  return ret;
}
