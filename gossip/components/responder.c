/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/components/responder.h"
#include "gossip/neighbor.h"
#include "gossip/node.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"

#define RESPONDER_LOGGER_ID "responder"
#define RESPONDER_TIMEOUT_SEC 1

/**
 * Private functions
 */

/**
 * Gets a transaction according to a request hash
 * - if null hash: gets a random tip
 * - if non-null hash: gets the corresponding transaction
 *
 * @param responder The responder
 * @param neighbor The requesting neighbor
 * @param hash The request hash
 * @param pack A stor pack to be filled with a transaction
 *
 * @return a status code
 */
static retcode_t get_transaction_for_request(responder_t const *const responder,
                                             neighbor_t *const neighbor,
                                             trit_array_t const *const hash,
                                             iota_stor_pack_t *const pack) {
  retcode_t ret = RC_OK;

  if (responder == NULL || neighbor == NULL || hash == NULL || pack == NULL) {
    return RC_NULL_PARAM;
  }

  // If the hash is null, a random tip was requested
  if (flex_trits_are_null(hash->trits, FLEX_TRIT_SIZE_243)) {
    log_debug(RESPONDER_LOGGER_ID, "Responding to random tip request\n");
    if (true) {
      // TODO random tip request
      neighbor->nbr_random_tx_req++;
    }
    // Else no tx to request, so no random tip will be sent as a reply.
  }
  // If the hash is non-null, a transaction was requested
  else {
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

/**
 * Responds to a request by:
 * - sending a transaction to the requesting neighbor or
 * - requesting a missing transaction
 *
 * @param responder The responder
 * @param neighbor The requesting neighbor
 * @param hash The request hash
 * @param pack A stor pack containing a transaction
 *
 * @return a status code
 */
static retcode_t respond_to_request(responder_t const *const responder,
                                    neighbor_t *const neighbor,
                                    flex_trit_t const *const hash,
                                    iota_stor_pack_t *const pack) {
  retcode_t ret = RC_OK;

  if (responder == NULL || neighbor == NULL || hash == NULL || pack == NULL) {
    return RC_NULL_PARAM;
  }

  if (pack->num_loaded != 0) {
    // If a transaction or a random tip was found, sends it back to the neighbor
    iota_transaction_t transaction = ((iota_transaction_t *)(pack->models))[0];
    flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];

    transaction_serialize_on_flex_trits(transaction, transaction_flex_trits);
    if ((ret = neighbor_send(responder->node, neighbor,
                             transaction_flex_trits)) != RC_OK) {
      log_warning(RESPONDER_LOGGER_ID, "Sending transaction failed\n");
      return ret;
    }
    // TODO Add transaction to cache
  } else {
    // If a transaction was requested but not found, requests it
    if (!flex_trits_are_null(hash, FLEX_TRIT_SIZE_243) &&
        rand_handle_probability() < responder->node->conf.p_propagate_request) {
      if ((ret = request_transaction(&responder->node->transaction_requester,
                                     hash, false)) != RC_OK) {
        log_warning(RESPONDER_LOGGER_ID, "Requesting transaction failed\n");
        return ret;
      }
    }
  }

  return RC_OK;
}

/**
 * Continuously looks for a transaction request from a responder queue and
 * process it
 *
 * @param responder The responder state
 */
static void *responder_routine(responder_t *const responder) {
  transaction_request_t *request_ptr = NULL;
  transaction_request_t request;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  trit_array_t hash = {.trits = NULL,
                       .num_trits = HASH_LENGTH_TRIT,
                       .num_bytes = FLEX_TRIT_SIZE_243,
                       .dynamic = 0};

  if (responder == NULL) {
    return NULL;
  }

  lock_handle_t lock_cond;
  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (responder->running) {
    if (responder_size(responder) == 0) {
      cond_handle_timedwait(&responder->cond, &lock_cond,
                            RESPONDER_TIMEOUT_SEC);
    }

    rw_lock_handle_wrlock(&responder->lock);
    request_ptr = transaction_request_queue_peek(responder->queue);
    if (request_ptr == NULL) {
      rw_lock_handle_unlock(&responder->lock);
      continue;
    }
    request = *request_ptr;
    transaction_request_queue_pop(&responder->queue);
    rw_lock_handle_unlock(&responder->lock);

    log_debug(RESPONDER_LOGGER_ID, "Responding to request\n");
    hash_pack_reset(&pack);
    hash.trits = request.hash;
    if (get_transaction_for_request(responder, request.neighbor, &hash,
                                    &pack) != RC_OK) {
      log_warning(RESPONDER_LOGGER_ID,
                  "Getting transaction for request failed\n");
    } else if (respond_to_request(responder, request.neighbor, request.hash,
                                  &pack) != RC_OK) {
      log_warning(RESPONDER_LOGGER_ID, "Replying to request failed\n");
    }
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

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
  responder->queue = NULL;
  rw_lock_handle_init(&responder->lock);
  cond_handle_init(&responder->cond);
  responder->node = node;
  responder->tangle = tangle;

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

  transaction_request_queue_free(&responder->queue);
  rw_lock_handle_destroy(&responder->lock);
  cond_handle_destroy(&responder->cond);
  responder->node = NULL;
  responder->tangle = NULL;

  logger_helper_destroy(RESPONDER_LOGGER_ID);

  return ret;
}

retcode_t responder_on_next(responder_t *const responder,
                            neighbor_t *const neighbor,
                            flex_trit_t const *const hash) {
  retcode_t ret = RC_OK;

  if (responder == NULL || neighbor == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&responder->lock);
  ret = transaction_request_queue_push(&responder->queue, neighbor, hash);
  rw_lock_handle_unlock(&responder->lock);

  if (ret != RC_OK) {
    log_warning(RESPONDER_LOGGER_ID,
                "Pushing transaction_request to responder queue failed\n");
    return ret;
  } else {
    cond_handle_signal(&responder->cond);
  }

  return RC_OK;
}

size_t responder_size(responder_t *const responder) {
  size_t size = 0;

  if (responder == NULL) {
    return 0;
  }

  rw_lock_handle_rdlock(&responder->lock);
  size = transaction_request_queue_count(responder->queue);
  rw_lock_handle_unlock(&responder->lock);

  return size;
}
