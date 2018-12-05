/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/components/transaction_requester.h"
#include "common/storage/sql/defs.h"
#include "consensus/tangle/tangle.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define REQUESTER_LOGGER_ID "requester"
#define REQUESTER_INTERVAL 100

/*
 * Private functions
 */

static void *transaction_requester_routine(
    transaction_requester_t *const transaction_requester) {
  if (transaction_requester == NULL) {
    return NULL;
  }

  while (transaction_requester->running) {
    fprintf(stderr, "Requester\n");
    sleep_ms(REQUESTER_INTERVAL);
  }

  return NULL;
}

/*
 * Public functions
 */

retcode_t requester_init(transaction_requester_t *const transaction_requester,
                         iota_gossip_conf_t *const conf,
                         tangle_t *const tangle) {
  if (transaction_requester == NULL || tangle == NULL || conf == NULL) {
    return RC_NULL_PARAM;
  }

  logger_helper_init(REQUESTER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(transaction_requester, 0, sizeof(transaction_requester_t));
  transaction_requester->conf = conf;
  transaction_requester->running = false;
  transaction_requester->milestones = NULL;
  transaction_requester->transactions = NULL;
  transaction_requester->tangle = tangle;
  rw_lock_handle_init(&transaction_requester->lock);

  return RC_OK;
}

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

retcode_t requester_destroy(
    transaction_requester_t *const transaction_requester) {
  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  } else if (transaction_requester->running) {
    return RC_STILL_RUNNING;
  }

  hash243_set_free(&transaction_requester->milestones);
  hash243_set_free(&transaction_requester->transactions);
  transaction_requester->tangle = NULL;
  rw_lock_handle_destroy(&transaction_requester->lock);
  logger_helper_destroy(REQUESTER_LOGGER_ID);

  return RC_OK;
}

retcode_t requester_get_requested_transactions(
    transaction_requester_t *const transaction_requester,
    hash243_set_t *const transactions) {
  retcode_t ret = RC_OK;

  if (transaction_requester == NULL || transactions == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&transaction_requester->lock);

  if ((ret = hash243_set_append(&transaction_requester->transactions,
                                transactions)) != RC_OK) {
    goto done;
  }
  if ((ret = hash243_set_append(&transaction_requester->milestones,
                                transactions)) != RC_OK) {
    goto done;
  }

done:
  rw_lock_handle_unlock(&transaction_requester->lock);
  return ret;
}

size_t requester_size(transaction_requester_t *const transaction_requester) {
  size_t size = 0;

  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&transaction_requester->lock);
  size = hash243_set_size(&transaction_requester->transactions) +
         hash243_set_size(&transaction_requester->milestones);
  rw_lock_handle_unlock(&transaction_requester->lock);

  return size;
}

bool requester_is_full(transaction_requester_t *const transaction_requester) {
  size_t size = 0;

  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&transaction_requester->lock);
  size = hash243_set_size(&transaction_requester->transactions);
  rw_lock_handle_unlock(&transaction_requester->lock);

  return size >= transaction_requester->conf->requester_queue_size;
}

retcode_t requester_clear_request(
    transaction_requester_t *const transaction_requester,
    flex_trit_t const *const hash) {
  if (transaction_requester == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&transaction_requester->lock);
  hash243_set_remove(&transaction_requester->milestones, hash);
  hash243_set_remove(&transaction_requester->transactions, hash);
  rw_lock_handle_unlock(&transaction_requester->lock);

  return RC_OK;
}

retcode_t request_transaction(
    transaction_requester_t *const transaction_requester,
    flex_trit_t *const hash, bool const is_milestone) {
  retcode_t ret = RC_OK;
  bool exists = false;
  trit_array_t const key = {.trits = hash,
                            .num_trits = HASH_LENGTH_TRIT,
                            .num_bytes = FLEX_TRIT_SIZE_243,
                            .dynamic = 0};

  if (transaction_requester == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  if (flex_trits_are_null(hash, FLEX_TRIT_SIZE_243)) {
    return RC_OK;
  }

  if ((ret = iota_tangle_transaction_exist(transaction_requester->tangle,
                                           TRANSACTION_FIELD_HASH, &key,
                                           &exists)) != RC_OK) {
    return ret;
  }
  if (exists) {
    return RC_OK;
  }

  fprintf(stderr, "Size %d\n", requester_size(transaction_requester));

  rw_lock_handle_wrlock(&transaction_requester->lock);

  if (is_milestone) {
    hash243_set_remove(&transaction_requester->transactions, hash);
    if ((ret = hash243_set_add(&transaction_requester->milestones, hash)) !=
        RC_OK) {
      goto done;
    }
  } else if (!hash243_set_contains(&transaction_requester->milestones, hash) &&
             hash243_set_size(&transaction_requester->transactions) <
                 transaction_requester->conf->requester_queue_size) {
    if ((ret = hash243_set_add(&transaction_requester->transactions, hash)) !=
        RC_OK) {
      goto done;
    }
  }

done:
  rw_lock_handle_unlock(&transaction_requester->lock);
  return ret;
}

retcode_t get_transaction_to_request(
    transaction_requester_t *const transaction_requester,
    flex_trit_t *const hash, bool const milestone) {
  retcode_t ret = RC_OK;
  hash243_set_t *request_set = NULL;
  hash243_set_t *backup_set = NULL;
  hash243_set_entry_t *iter = NULL;
  hash243_set_entry_t *tmp = NULL;
  trit_array_t key = {.trits = NULL,
                      .num_trits = HASH_LENGTH_TRIT,
                      .num_bytes = FLEX_TRIT_SIZE_243,
                      .dynamic = 0};
  bool exists = false;

  if (transaction_requester == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  request_set = milestone ? &transaction_requester->milestones
                          : &transaction_requester->transactions;
  backup_set = milestone ? &transaction_requester->transactions
                         : &transaction_requester->milestones;

  rw_lock_handle_wrlock(&transaction_requester->lock);

  request_set = hash243_set_size(request_set) != 0 ? request_set : backup_set;

  HASH_ITER(hh, *request_set, iter, tmp) {
    key.trits = iter->hash;
    if ((ret = iota_tangle_transaction_exist(transaction_requester->tangle,
                                             TRANSACTION_FIELD_HASH, &key,
                                             &exists)) != RC_OK) {
      goto done;
    }
    if (exists) {
      hash243_set_remove_entry(request_set, iter);
    } else {
      memcpy(hash, iter->hash, FLEX_TRIT_SIZE_243);
      break;
    }
  }

  if (hash243_set_size(request_set) == 0) {
    memset(hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }

  if (rand_handle_probability() <
          transaction_requester->conf->p_remove_request &&
      request_set != &transaction_requester->milestones) {
    hash243_set_remove(request_set, iter->hash);
  }

done:
  rw_lock_handle_unlock(&transaction_requester->lock);
  return ret;
}
