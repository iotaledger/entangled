/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/components/responder.h"
#include "ciri/core.h"
#include "common/storage/sql/defs.h"
#include "gossip/neighbor.h"
#include "utils/containers/queues/concurrent_queue_packet.h"
#include "utils/containers/queues/concurrent_queue_transaction_request.h"
#include "utils/logger_helper.h"

#define RESPONDER_COMPONENT_LOGGER_ID "responder_component"

static retcode_t random_tip_request(transaction_request_t *const request) {
  // TODO(thibault): Random tip request
  request->neighbor->nbr_random_tx_req++;
  return RC_OK;
}

static retcode_t regular_transaction_request(
    responder_state_t *const state, transaction_request_t *const request,
    iota_transaction_t *const tx) {
  retcode_t ret = RC_OK;

  if ((*tx = transaction_new()) == NULL) {
    return RC_RESPONDER_COMPONENT_OOM;
  }

  iota_stor_pack_t pack = {.models = (void **)tx,
                           .capacity = 1,
                           .num_loaded = 0,
                           .insufficient_capacity = false};
  if ((ret = iota_tangle_transaction_load(state->tangle, TRANSACTION_FIELD_HASH,
                                          request->hash, &pack))) {
    return ret;
  }
  if (pack.num_loaded == 0) {
    transaction_free(*tx);
    *tx = NULL;
  }
  return RC_OK;
}

static retcode_t get_transaction_for_request(
    responder_state_t *const state, transaction_request_t *const request,
    iota_transaction_t *const tx) {
  if (state == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_STATE;
  } else if (request == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_REQ;
  } else if (request->neighbor == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_NEIGHBOR;
  } else if (tx == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_TX;
  }

  if (trit_array_is_null(request->hash)) {
    log_debug(RESPONDER_COMPONENT_LOGGER_ID,
              "Responding to random tip request\n");
    return random_tip_request(request);
  }  // else
  log_debug(RESPONDER_COMPONENT_LOGGER_ID,
            "Responding to regular transaction request\n");
  return regular_transaction_request(state, request, tx);
}

static retcode_t reply_to_request(responder_state_t *const state,
                                  transaction_request_t *const request,
                                  iota_transaction_t const tx) {
  retcode_t ret = RC_OK;

  if (state == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_STATE;
  } else if (request == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_REQ;
  } else if (request->neighbor == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_NEIGHBOR;
  }

  if (tx != NULL) {
    // Send transaction back to neighbor
    iota_packet_t packet = {{0}};
    iota_packet_set_transaction(&packet, tx);
    if ((ret = neighbor_send(state->node, request->neighbor, &packet))) {
      return ret;
    }
  } else {
    // Transaction not found
    // TODO(thibault): Randomly doesn't propagate request
    if (trit_array_is_null(request->hash) == false) {
      // Request is an actual missing transaction
      if ((ret = request_transaction(state->node->requester, request->hash))) {
        return ret;
      }
    }
  }
  return RC_OK;
}

static void *responder_routine(responder_state_t *const state) {
  transaction_request_t request;
  iota_transaction_t tx = NULL;

  if (state == NULL) {
    return NULL;
  }

  while (state->running) {
    if (CQ_POP(state->queue, &request) == CQ_SUCCESS) {
      log_debug(RESPONDER_COMPONENT_LOGGER_ID, "Responding to request\n");
      if (get_transaction_for_request(state, &request, &tx)) {
        log_warning(RESPONDER_COMPONENT_LOGGER_ID,
                    "Getting transaction for request failed\n");
        continue;
      }
      if (reply_to_request(state, &request, tx)) {
        log_warning(RESPONDER_COMPONENT_LOGGER_ID,
                    "Replying to request failed\n");
        continue;
      }
    }
  }
  return NULL;
}

retcode_t responder_init(responder_state_t *const state, node_t *const node,
                         tangle_t *const tangle) {
  if (state == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_STATE;
  } else if (node == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_NODE;
  }

  logger_helper_init(RESPONDER_COMPONENT_LOGGER_ID, LOGGER_DEBUG, true);
  memset(state, 0, sizeof(responder_state_t));
  state->running = false;
  state->node = node;
  state->tangle = tangle;

  log_debug(RESPONDER_COMPONENT_LOGGER_ID, "Initializing responder queue\n");
  if (CQ_INIT(transaction_request_t, state->queue) != CQ_SUCCESS) {
    log_critical(RESPONDER_COMPONENT_LOGGER_ID,
                 "Initializing responder queue failed\n");
    return RC_RESPONDER_COMPONENT_FAILED_INIT_QUEUE;
  }

  return RC_OK;
}

retcode_t responder_start(responder_state_t *const state) {
  if (state == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_STATE;
  }

  log_info(RESPONDER_COMPONENT_LOGGER_ID, "Spawning responder thread\n");
  state->running = true;
  if (thread_handle_create(&state->thread, (thread_routine_t)responder_routine,
                           state) != 0) {
    log_critical(RESPONDER_COMPONENT_LOGGER_ID,
                 "Spawning responder thread failed\n");
    return RC_RESPONDER_COMPONENT_FAILED_THREAD_SPAWN;
  }
  return RC_OK;
}

retcode_t responder_on_next(responder_state_t *const state,
                            neighbor_t *const neighbor,
                            trit_array_p const hash) {
  if (state == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_STATE;
  }

  if (CQ_PUSH(state->queue, ((transaction_request_t){neighbor, hash})) !=
      CQ_SUCCESS) {
    log_warning(RESPONDER_COMPONENT_LOGGER_ID,
                "Adding request to responder queue failed\n");
    return RC_RESPONDER_COMPONENT_FAILED_ADD_QUEUE;
  }
  return RC_OK;
}

retcode_t responder_stop(responder_state_t *const state) {
  retcode_t ret = RC_OK;

  if (state == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_STATE;
  } else if (state->running == false) {
    return RC_OK;
  }

  log_info(RESPONDER_COMPONENT_LOGGER_ID, "Shutting down responder thread\n");
  state->running = false;
  if (thread_handle_join(state->thread, NULL) != 0) {
    log_error(RESPONDER_COMPONENT_LOGGER_ID,
              "Shutting down responder thread failed\n");
    ret = RC_RESPONDER_COMPONENT_FAILED_THREAD_JOIN;
  }
  return ret;
}

retcode_t responder_destroy(responder_state_t *const state) {
  retcode_t ret = RC_OK;

  if (state == NULL) {
    return RC_RESPONDER_COMPONENT_NULL_STATE;
  } else if (state->running) {
    return RC_RESPONDER_COMPONENT_STILL_RUNNING;
  }

  log_debug(RESPONDER_COMPONENT_LOGGER_ID, "Destroying responder queue\n");
  if (CQ_DESTROY(transaction_request_t, state->queue) != CQ_SUCCESS) {
    log_error(RESPONDER_COMPONENT_LOGGER_ID,
              "Destroying responder queue failed\n");
    ret = RC_RESPONDER_COMPONENT_FAILED_DESTROY_QUEUE;
  }
  logger_helper_destroy(RESPONDER_COMPONENT_LOGGER_ID);
  return ret;
}
