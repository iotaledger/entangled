/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/components/responder.h"
#include "ciri/core.h"
#include "ciri/node.h"
#include "common/storage/sql/defs.h"
#include "common/storage/storage.h"
#include "utils/logger_helper.h"

#define RESPONDER_COMPONENT_LOGGER_ID "responder_component"

static bool get_transaction_to_reply(responder_state_t *const state,
                                     hash_request_t *const request,
                                     iota_transaction_t *const tx) {
  if (state == NULL) {
    return false;
  }
  if (request == NULL) {
    return false;
  }
  if (request->neighbor == NULL) {
    return false;
  }
  if (tx == NULL) {
    return false;
  }
  if (trit_array_is_null(request->hash)) {
    // Random tip request
    // TODO(thibault): Random tip request
    request->neighbor->random_tx_req++;
  } else {
    // Regular transaction request
    iota_transactions_pack pack;
    if ((*tx = transaction_new()) == NULL) {
      return false;
    }
    pack.txs = tx;
    pack.num_loaded = 0;
    pack.txs_capacity = 1;
    if (iota_stor_load(&state->node->core->db_conn, COL_HASH, request->hash,
                       &pack)) {
      return false;
    }
    // if (pack.num_loaded == 0) {
    //   return false;
    // }
  }
  return true;
}

bool reply_to_request(responder_state_t *const state,
                      hash_request_t *const request,
                      iota_transaction_t const tx) {
  if (state == NULL) {
    return false;
  }
  if (request == NULL) {
    return false;
  }
  if (request->neighbor == NULL) {
    return false;
  }
  printf("Hello %p\n", tx);
  if (tx != NULL) {
    // Send transaction back to neighbor
    iota_packet_t packet;
    if (neighbor_send(state->node, request->neighbor, &packet)) {
      return false;
    }
  } else {
    // Transaction not found
    // TODO(thibault): Randomly doesn't propagate request
    if (trit_array_is_null(request->hash) == false) {
      // Request is an actual missing transaction
      if (request_transaction(&state->node->requester, request->hash) ==
          false) {
        return false;
      }
    }
  }
  return true;
}

static void *responder_routine(responder_state_t *const state) {
  hash_request_t request;
  iota_transaction_t tx = NULL;

  if (state == NULL) {
    return NULL;
  }
  while (state->running) {
    if (state->queue->vtable->pop(state->queue, &request) ==
        CONCURRENT_QUEUE_SUCCESS) {
      log_debug(RESPONDER_COMPONENT_LOGGER_ID, "Responding to request\n");

      if (get_transaction_to_reply(state, &request, &tx) == false) {
        continue;
      }

      if (reply_to_request(state, &request, tx) == false) {
        continue;
      }
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

bool responder_on_next(responder_state_t *const state,
                       neighbor_t *const neighbor, trit_array_p const hash) {
  if (state == NULL) {
    return false;
  }
  if (state->queue->vtable->push(state->queue,
                                 (hash_request_t){neighbor, hash}) !=
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
