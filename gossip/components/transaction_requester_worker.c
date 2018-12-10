/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/components/transaction_requester_worker.h"
#include "common/storage/sql/defs.h"
#include "consensus/tangle/tangle.h"
#include "gossip/node.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define REQUESTER_LOGGER_ID "requester"
#define REQUESTER_INTERVAL 10
#define REQUESTER_THRESHOLD 100

/*
 * Private functions
 */

static void *transaction_requester_routine(
    transaction_requester_t *const transaction_requester) {
  neighbor_t *iter = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t transaction[FLEX_TRIT_SIZE_8019];
  retcode_t ret = RC_OK;
  trit_array_t key = {.trits = hash,
                      .num_trits = HASH_LENGTH_TRIT,
                      .num_bytes = FLEX_TRIT_SIZE_243,
                      .dynamic = 0};
  DECLARE_PACK_SINGLE_TX(tx, txp, pack);

  if (transaction_requester == NULL) {
    return NULL;
  }

  while (transaction_requester->running) {
    if (requester_size(transaction_requester) <= REQUESTER_THRESHOLD) {
      goto sleep;
    }
    tips_cache_random_tip(&transaction_requester->node->tips, hash);
    if (flex_trits_are_null(hash, FLEX_TRIT_SIZE_243)) {
      memset(transaction, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_8019);
    } else {
      hash_pack_reset(&pack);
      ret = iota_tangle_transaction_load(transaction_requester->tangle,
                                         TRANSACTION_FIELD_HASH, &key, &pack);
      if (ret == RC_OK && pack.num_loaded != 0) {
        transaction_serialize_on_flex_trits(txp, transaction);
      } else {
        memset(transaction, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_8019);
      }
    }
    rw_lock_handle_rdlock(&transaction_requester->node->neighbors_lock);
    LL_FOREACH(transaction_requester->node->neighbors, iter) {
      if (neighbor_send(transaction_requester->node, iter, transaction) !=
          RC_OK) {
        log_warning(REQUESTER_LOGGER_ID, "Sending request failed\n");
      }
    }
    rw_lock_handle_unlock(&transaction_requester->node->neighbors_lock);
  sleep:
    sleep_ms(REQUESTER_INTERVAL);
  }

  return NULL;
}

/*
 * Public functions
 */

retcode_t requester_start(
    transaction_requester_t *const transaction_requester) {
  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(REQUESTER_LOGGER_ID, "Spawning transaction requester thread\n");
  transaction_requester->running = true;
  if (thread_handle_create(&transaction_requester->thread,
                           (thread_routine_t)transaction_requester_routine,
                           transaction_requester) != 0) {
    return RC_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t requester_stop(transaction_requester_t *const transaction_requester) {
  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  } else if (transaction_requester->running == false) {
    return RC_OK;
  }

  log_info(REQUESTER_LOGGER_ID, "Shutting down transaction requester thread\n");
  transaction_requester->running = false;
  if (thread_handle_join(transaction_requester->thread, NULL) != 0) {
    log_error(REQUESTER_LOGGER_ID,
              "Shutting down transaction requester thread failed\n");
    return RC_FAILED_THREAD_JOIN;
  }

  return RC_OK;
}
