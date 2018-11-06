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
#include "utils/containers/queues/concurrent_queue_transaction_request.h"
#include "utils/logger_helper.h"

#define RESPONDER_LOGGER_ID "responder"

static retcode_t random_tip_request(transaction_request_t *const request) {
  if (request == NULL) {
    return RC_NULL_PARAM;
  }
  // TODO(thibault): Random tip request
  request->neighbor->nbr_random_tx_req++;
  return RC_OK;
}

static retcode_t get_transaction_for_request(
    responder_t *const processor, transaction_request_t *const request,
    iota_stor_pack_t *const pack) {
  retcode_t ret = RC_OK;

  if (processor == NULL || request == NULL || request->neighbor == NULL ||
      pack == NULL) {
    return RC_NULL_PARAM;
  }

  trit_array_t key = {.trits = request->hash,
                      .num_trits = HASH_LENGTH_TRIT,
                      .num_bytes = FLEX_TRIT_SIZE_243,
                      .dynamic = 0};
  if (flex_trits_is_null(request->hash, FLEX_TRIT_SIZE_243)) {
    log_debug(RESPONDER_LOGGER_ID, "Responding to random tip request\n");
    return random_tip_request(request);
  }
  log_debug(RESPONDER_LOGGER_ID, "Responding to regular transaction request\n");
  if ((ret = iota_tangle_transaction_load(
           processor->tangle, TRANSACTION_FIELD_HASH, &key, pack))) {
    return ret;
  }
  return ret;
}

static retcode_t reply_to_request(responder_t *const processor,
                                  transaction_request_t *const request,
                                  iota_stor_pack_t *const pack) {
  retcode_t ret = RC_OK;

  if (processor == NULL || request == NULL || request->neighbor == NULL ||
      pack == NULL) {
    return RC_NULL_PARAM;
  }
  if (pack->num_loaded != 0) {
    // Send transaction back to neighbor
    flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
    iota_transaction_t tx = ((iota_transaction_t *)(pack->models))[0];
    transaction_serialize_on_flex_trits(tx, tx_trits);
    if ((ret = neighbor_send(processor->node, request->neighbor, tx_trits)) !=
        RC_OK) {
      return ret;
    }
  } else {
    // Transaction not found
    // TODO(thibault): Randomly doesn't propagate request
    if (!flex_trits_is_null(request->hash, FLEX_TRIT_SIZE_243)) {
      // Request is an actual missing transaction
      if ((ret = request_transaction(&processor->node->transaction_requester,
                                     request->hash, false)) != RC_OK) {
        return ret;
      }
    }
  }
  return RC_OK;
}

static void *responder_routine(responder_t *const processor) {
  transaction_request_t request;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);

  if (processor == NULL) {
    return NULL;
  }

  while (processor->running) {
    if (CQ_POP(processor->queue, &request) == CQ_SUCCESS) {
      log_debug(RESPONDER_LOGGER_ID, "Responding to request\n");
      hash_pack_reset(&pack);
      if (get_transaction_for_request(processor, &request, &pack)) {
        log_warning(RESPONDER_LOGGER_ID,
                    "Getting transaction for request failed\n");
        continue;
      }
      if (reply_to_request(processor, &request, &pack)) {
        log_warning(RESPONDER_LOGGER_ID, "Replying to request failed\n");
        continue;
      }
    }
  }
  return NULL;
}

retcode_t responder_init(responder_t *const processor, node_t *const node,
                         tangle_t *const tangle) {
  if (processor == NULL || node == NULL || tangle == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_init(RESPONDER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(processor, 0, sizeof(responder_t));
  processor->running = false;
  processor->node = node;
  processor->tangle = tangle;

  log_debug(RESPONDER_LOGGER_ID, "Initializing responder queue\n");
  if (CQ_INIT(transaction_request_t, processor->queue) != CQ_SUCCESS) {
    log_critical(RESPONDER_LOGGER_ID, "Initializing responder queue failed\n");
    return RC_RESPONDER_COMPONENT_FAILED_INIT_QUEUE;
  }

  return RC_OK;
}

retcode_t responder_start(responder_t *const processor) {
  if (processor == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(RESPONDER_LOGGER_ID, "Spawning responder thread\n");
  processor->running = true;
  if (thread_handle_create(&processor->thread,
                           (thread_routine_t)responder_routine,
                           processor) != 0) {
    log_critical(RESPONDER_LOGGER_ID, "Spawning responder thread failed\n");
    return RC_FAILED_THREAD_SPAWN;
  }
  return RC_OK;
}

retcode_t responder_stop(responder_t *const processor) {
  retcode_t ret = RC_OK;

  if (processor == NULL) {
    return RC_NULL_PARAM;
  } else if (processor->running == false) {
    return RC_OK;
  }

  log_info(RESPONDER_LOGGER_ID, "Shutting down responder thread\n");
  processor->running = false;
  if (thread_handle_join(processor->thread, NULL) != 0) {
    log_error(RESPONDER_LOGGER_ID, "Shutting down responder thread failed\n");
    ret = RC_FAILED_THREAD_JOIN;
  }
  return ret;
}

retcode_t responder_destroy(responder_t *const processor) {
  retcode_t ret = RC_OK;

  if (processor == NULL) {
    return RC_NULL_PARAM;
  } else if (processor->running) {
    return RC_STILL_RUNNING;
  }

  log_debug(RESPONDER_LOGGER_ID, "Destroying responder queue\n");
  if (CQ_DESTROY(transaction_request_t, processor->queue) != CQ_SUCCESS) {
    log_error(RESPONDER_LOGGER_ID, "Destroying responder queue failed\n");
    ret = RC_RESPONDER_COMPONENT_FAILED_DESTROY_QUEUE;
  }
  logger_helper_destroy(RESPONDER_LOGGER_ID);
  return ret;
}

retcode_t responder_on_next(responder_t *const processor,
                            neighbor_t *const neighbor,
                            flex_trit_t const *const hash) {
  if (processor == NULL || neighbor == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  transaction_request_t req;
  req.neighbor = neighbor;
  memcpy(req.hash, hash, FLEX_TRIT_SIZE_243);

  if (CQ_PUSH(processor->queue, req) != CQ_SUCCESS) {
    log_warning(RESPONDER_LOGGER_ID,
                "Adding request to responder queue failed\n");
    return RC_RESPONDER_COMPONENT_FAILED_ADD_QUEUE;
  }
  return RC_OK;
}
