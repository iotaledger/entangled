/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/components/responder.h"
#include "consensus/tangle/tangle.h"
#include "gossip/neighbor.h"
#include "gossip/node.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"

#define RESPONDER_LOGGER_ID "responder"

/**
 * Private functions
 */

/**
 * Gets a transaction according to a request hash
 * - if null hash: gets a random tip
 * - if non-null hash: gets the corresponding transaction
 *
 * @param responder The responder
 * @param tangle A tangle
 * @param neighbor The requesting neighbor
 * @param hash The request hash
 * @param pack A stor pack to be filled with a transaction
 *
 * @return a status code
 */
static retcode_t get_transaction_for_request(responder_t const *const responder,
                                             tangle_t *const tangle,
                                             neighbor_t *const neighbor,
                                             flex_trit_t const *const hash,
                                             iota_stor_pack_t *const pack) {
  retcode_t ret = RC_OK;

  if (responder == NULL || neighbor == NULL || hash == NULL || pack == NULL) {
    return RC_NULL_PARAM;
  }

  // If the hash is null, a random tip was requested
  if (flex_trits_are_null(hash, FLEX_TRIT_SIZE_243)) {
    flex_trit_t tip[FLEX_TRIT_SIZE_243];

    log_debug(RESPONDER_LOGGER_ID, "Responding to random tip request\n");
    if (rand_handle_probability() < responder->node->conf.p_reply_random_tip &&
        !requester_is_empty(&responder->node->transaction_requester)) {
      neighbor->nbr_random_tx_req++;
      if ((ret = tips_cache_random_tip(&responder->node->tips, tip)) != RC_OK) {
        return ret;
      }
      if ((ret = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH,
                                              tip, pack)) != RC_OK ||
          pack->num_loaded == 0) {
        return ret;
      }
    }
    // Else no tx to request, so no random tip will be sent as a reply.
  }
  // If the hash is non-null, a transaction was requested
  else {
    log_debug(RESPONDER_LOGGER_ID,
              "Responding to regular transaction request\n");
    if ((ret = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH,
                                            hash, pack)) != RC_OK) {
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
 * @param tangle A tangle
 * @param neighbor The requesting neighbor
 * @param hash The request hash
 * @param pack A stor pack containing a transaction
 *
 * @return a status code
 */
static retcode_t respond_to_request(responder_t const *const responder,
                                    tangle_t *const tangle,
                                    neighbor_t *const neighbor,
                                    flex_trit_t const *const hash,
                                    iota_stor_pack_t *const pack) {
  retcode_t ret = RC_OK;

  if (responder == NULL || neighbor == NULL || hash == NULL || pack == NULL) {
    return RC_NULL_PARAM;
  }

  if (pack->num_loaded != 0) {
    // If a transaction or a random tip was found, sends it back to the neighbor
    iota_transaction_t *transaction =
        ((iota_transaction_t **)(pack->models))[0];
    flex_trit_t transaction_flex_trits[FLEX_TRIT_SIZE_8019];

    transaction_serialize_on_flex_trits(transaction, transaction_flex_trits);
    if ((ret = neighbor_send(responder->node, tangle, neighbor,
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
                                     tangle, hash, false)) != RC_OK) {
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
  transaction_request_t request;
  bool has_dequeued = false;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, pack);
  connection_config_t db_conf = {.db_path = responder->node->conf.db_path};
  tangle_t tangle;

  if (responder == NULL) {
    return NULL;
  }

  if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
    log_critical(RESPONDER_LOGGER_ID,
                 "Initializing tangle connection failed\n");
    return NULL;
  }

  while (responder->running) {
    while (responder->running && LF_MPMC_QUEUE_IS_EMPTY(&responder->queue)) {
      ck_pr_stall();
    }

    if (lf_mpmc_queue_transaction_request_t_trydequeue(
            &responder->queue, &request, &has_dequeued) != RC_OK) {
      log_warning(RESPONDER_LOGGER_ID,
                  "Dequeuing transaction request failed\n");
      continue;
    }

    if (!has_dequeued) {
      continue;
    }

    log_debug(RESPONDER_LOGGER_ID, "Responding to request\n");
    hash_pack_reset(&pack);
    if (get_transaction_for_request(responder, &tangle, request.neighbor,
                                    request.hash, &pack) != RC_OK) {
      log_warning(RESPONDER_LOGGER_ID,
                  "Getting transaction for request failed\n");
    } else if (respond_to_request(responder, &tangle, request.neighbor,
                                  request.hash, &pack) != RC_OK) {
      log_warning(RESPONDER_LOGGER_ID, "Replying to request failed\n");
    }
  }

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(RESPONDER_LOGGER_ID, "Destroying tangle connection failed\n");
  }

  return NULL;
}

/**
 * Public functions
 */

retcode_t responder_init(responder_t *const responder, node_t *const node) {
  retcode_t ret = RC_OK;

  if (responder == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_enable(RESPONDER_LOGGER_ID, LOGGER_DEBUG, true);

  responder->running = false;
  responder->node = node;

  if ((ret = lf_mpmc_queue_transaction_request_t_init(
           &responder->queue, sizeof(transaction_request_t))) != RC_OK) {
    log_critical(RESPONDER_LOGGER_ID, "Initializing queue failed\n");
    return ret;
  }

  return ret;
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

  responder->node = NULL;

  if ((ret = lf_mpmc_queue_transaction_request_t_destroy(&responder->queue)) !=
      RC_OK) {
    log_critical(RESPONDER_LOGGER_ID, "Destroying queue failed\n");
  }

  logger_helper_release(RESPONDER_LOGGER_ID);

  return ret;
}

retcode_t responder_on_next(responder_t *const responder,
                            neighbor_t *const neighbor,
                            flex_trit_t const *const hash) {
  retcode_t ret = RC_OK;
  transaction_request_t request;

  if (responder == NULL || neighbor == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  request.neighbor = neighbor;
  memcpy(request.hash, hash, FLEX_TRIT_SIZE_243);
  if ((ret = lf_mpmc_queue_transaction_request_t_enqueue(&responder->queue,
                                                         &request)) != RC_OK) {
    log_warning(RESPONDER_LOGGER_ID, "Enqueuing transaction request failed\n");
    return ret;
  }

  return ret;
}
