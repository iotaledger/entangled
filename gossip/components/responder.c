/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/components/responder.h"
#include "gossip/neighbor.h"
#include "gossip/node.h"
#include "utils/containers/queues/concurrent_queue_transaction_request.h"
#include "utils/logger_helper.h"

#define RESPONDER_LOGGER_ID "responder"

/**
 * Private functions
 */

static retcode_t get_transaction_for_request(responder_t const *const responder,
                                             neighbor_t *const neighbor,
                                             trit_array_t const *const hash,
                                             iota_stor_pack_t *const pack) {
  retcode_t ret = RC_OK;

  if (responder == NULL || neighbor == NULL || hash == NULL || pack == NULL) {
    return RC_NULL_PARAM;
  }

  if (flex_trits_is_null(hash->trits, FLEX_TRIT_SIZE_243)) {
    log_debug(RESPONDER_LOGGER_ID, "Responding to random tip request\n");
    if (true) {
      neighbor->nbr_random_tx_req++;
    }
    // Else no tx to request, so no random tip will be sent as a reply.
  } else {
    log_debug(RESPONDER_LOGGER_ID,
              "Responding to regular transaction request\n");
    if ((ret = iota_tangle_transaction_load(
             responder->tangle, TRANSACTION_FIELD_HASH, hash, pack)) != RC_OK) {
      log_warning(RESPONDER_LOGGER_ID, "Loading transaction failed\n");
      return ret;
    }
  }

  return ret;
}

static retcode_t respond_to_request(responder_t const *const responder,
                                    transaction_request_t *const request,
                                    iota_stor_pack_t *const pack) {
  retcode_t ret = RC_OK;

  if (responder == NULL || request == NULL || request->neighbor == NULL ||
      pack == NULL) {
    return RC_NULL_PARAM;
  }

  if (pack->num_loaded != 0) {
    // Send transaction back to neighbor
    iota_transaction_t transaction = ((iota_transaction_t *)(pack->models))[0];
    flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];

    transaction_serialize_on_flex_trits(transaction, transaction_flex_trits);
    if ((ret = neighbor_send(responder->node, request->neighbor,
                             transaction_flex_trits)) != RC_OK) {
      log_warning(RESPONDER_LOGGER_ID, "Sending transaction failed\n");
      return ret;
    }
    // TODO Add transaction to cache
  } else {
    // Transaction not found
    if (!flex_trits_is_null(request->hash, FLEX_TRIT_SIZE_243) &&
        ((double)rand() / (double)RAND_MAX) <
            responder->node->conf.p_propagate_request) {
      // Request is an actual missing transaction
      if ((ret = request_transaction(&responder->node->transaction_requester,
                                     request->hash, false)) != RC_OK) {
        log_warning(RESPONDER_LOGGER_ID, "Requesting transaction failed\n");
        return ret;
      }
    }
  }

  return RC_OK;
}

static void *responder_routine(responder_t const *const responder) {
  transaction_request_t request;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  trit_array_t hash = {.trits = NULL,
                       .num_trits = HASH_LENGTH_TRIT,
                       .num_bytes = FLEX_TRIT_SIZE_243,
                       .dynamic = 0};

  if (responder == NULL) {
    return NULL;
  }

  while (responder->running) {
    if (CQ_POP(responder->queue, &request) == CQ_SUCCESS) {
      log_debug(RESPONDER_LOGGER_ID, "Responding to request\n");
      hash_pack_reset(&pack);
      hash.trits = request.hash;
      if (get_transaction_for_request(responder, request.neighbor, &hash,
                                      &pack) != RC_OK) {
        log_warning(RESPONDER_LOGGER_ID,
                    "Getting transaction for request failed\n");
        continue;
      }
      if (respond_to_request(responder, &request, &pack) != RC_OK) {
        log_warning(RESPONDER_LOGGER_ID, "Replying to request failed\n");
        continue;
      }
    }
  }

  return NULL;
}

/**
 * Public functions
 */

retcode_t responder_init(responder_t *const responder, node_t *const node,
                         tangle_t *const tangle) {
  if (responder == NULL || node == NULL || tangle == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_init(RESPONDER_LOGGER_ID, LOGGER_DEBUG, true);
  responder->running = false;
  responder->node = node;
  responder->tangle = tangle;

  log_debug(RESPONDER_LOGGER_ID, "Initializing responder queue\n");
  if (CQ_INIT(transaction_request_t, responder->queue) != CQ_SUCCESS) {
    log_critical(RESPONDER_LOGGER_ID, "Initializing responder queue failed\n");
    return RC_RESPONDER_COMPONENT_FAILED_INIT_QUEUE;
  }

  return RC_OK;
}

retcode_t responder_start(responder_t *const responder) {
  if (responder == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(RESPONDER_LOGGER_ID, "Spawning responder thread\n");
  responder->running = true;
  if (thread_handle_create(&responder->thread,
                           (thread_routine_t)responder_routine,
                           responder) != 0) {
    log_critical(RESPONDER_LOGGER_ID, "Spawning responder thread failed\n");
    return RC_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t responder_stop(responder_t *const responder) {
  retcode_t ret = RC_OK;

  if (responder == NULL) {
    return RC_NULL_PARAM;
  } else if (responder->running == false) {
    return RC_OK;
  }

  log_info(RESPONDER_LOGGER_ID, "Shutting down responder thread\n");
  responder->running = false;
  if (thread_handle_join(responder->thread, NULL) != 0) {
    log_error(RESPONDER_LOGGER_ID, "Shutting down responder thread failed\n");
    ret = RC_FAILED_THREAD_JOIN;
  }

  return ret;
}

retcode_t responder_destroy(responder_t *const responder) {
  retcode_t ret = RC_OK;

  if (responder == NULL) {
    return RC_NULL_PARAM;
  } else if (responder->running) {
    return RC_STILL_RUNNING;
  }

  log_debug(RESPONDER_LOGGER_ID, "Destroying responder queue\n");
  if (CQ_DESTROY(transaction_request_t, responder->queue) != CQ_SUCCESS) {
    log_error(RESPONDER_LOGGER_ID, "Destroying responder queue failed\n");
    ret = RC_RESPONDER_COMPONENT_FAILED_DESTROY_QUEUE;
  }
  logger_helper_destroy(RESPONDER_LOGGER_ID);

  return ret;
}

retcode_t responder_on_next(responder_t *const responder,
                            neighbor_t *const neighbor,
                            flex_trit_t const *const hash) {
  if (responder == NULL || neighbor == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  transaction_request_t req;
  req.neighbor = neighbor;
  memcpy(req.hash, hash, FLEX_TRIT_SIZE_243);

  if (CQ_PUSH(responder->queue, req) != CQ_SUCCESS) {
    log_warning(RESPONDER_LOGGER_ID,
                "Adding request to responder queue failed\n");
    return RC_RESPONDER_COMPONENT_FAILED_ADD_QUEUE;
  }

  return RC_OK;
}
