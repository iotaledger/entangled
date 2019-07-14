/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/pipeline/transaction_requester_worker.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/node/node.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define REQUESTER_LOGGER_ID "requester"
#define REQUESTER_INTERVAL_MS 10ULL

static logger_id_t logger_id;

/*
 * Private functions
 */

static void *transaction_requester_routine(transaction_requester_t *const transaction_requester) {
  neighbor_t *neighbor = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t transaction[FLEX_TRIT_SIZE_8019];
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_TX(tx, txp, pack);
  tangle_t tangle;

  if (transaction_requester == NULL) {
    return NULL;
  }

  {
    connection_config_t db_conf = {.db_path = transaction_requester->node->conf.tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return NULL;
    }
  }

  lock_handle_t lock_cond;
  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (transaction_requester->running) {
    if (requester_is_empty(transaction_requester)) {
      goto sleep;
    }
    tips_cache_random_tip(&transaction_requester->node->tips, hash);
    if (flex_trits_are_null(hash, FLEX_TRIT_SIZE_243)) {
      memset(transaction, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_8019);
    } else {
      hash_pack_reset(&pack);
      ret = iota_tangle_transaction_load(&tangle, TRANSACTION_FIELD_HASH, hash, &pack);
      if (ret == RC_OK && pack.num_loaded != 0) {
        transaction_serialize_on_flex_trits(txp, transaction);
      } else {
        memset(transaction, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_8019);
      }
    }
    rw_lock_handle_rdlock(&transaction_requester->node->router.neighbors_lock);
    NEIGHBORS_FOREACH(transaction_requester->node->router.neighbors, neighbor) {
      if (neighbor->endpoint.stream != NULL) {
        if (neighbor_send_trits(transaction_requester->node, &tangle, neighbor, transaction) != RC_OK) {
          log_warning(logger_id, "Sending request failed\n");
        }
      }
    }
    rw_lock_handle_unlock(&transaction_requester->node->router.neighbors_lock);
  sleep:
    cond_handle_timedwait(&transaction_requester->cond, &lock_cond, REQUESTER_INTERVAL_MS);
  }

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  return NULL;
}

/*
 * Public functions
 */

retcode_t requester_start(transaction_requester_t *const transaction_requester) {
  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(REQUESTER_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(logger_id, "Spawning transaction requester thread\n");
  transaction_requester->running = true;
  if (thread_handle_create(&transaction_requester->thread, (thread_routine_t)transaction_requester_routine,
                           transaction_requester) != 0) {
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t requester_stop(transaction_requester_t *const transaction_requester) {
  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  } else if (transaction_requester->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down transaction requester thread\n");
  transaction_requester->running = false;
  cond_handle_signal(&transaction_requester->cond);
  if (thread_handle_join(transaction_requester->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down transaction requester thread failed\n");
    return RC_THREAD_JOIN;
  }

  return RC_OK;
}
