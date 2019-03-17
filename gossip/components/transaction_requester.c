/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/components/transaction_requester.h"
#include "common/storage/sql/defs.h"
#include "consensus/tangle/tangle.h"
#include "gossip/node.h"
#include "utils/handles/rand.h"
#include "utils/logger_helper.h"

#define REQUESTER_LOGGER_ID "requester"

static logger_id_t logger_id;

/*
 * Public functions
 */

retcode_t requester_init(transaction_requester_t *const transaction_requester, node_t *const node) {
  if (transaction_requester == NULL || node == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(REQUESTER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(transaction_requester, 0, sizeof(transaction_requester_t));
  transaction_requester->node = node;
  transaction_requester->running = false;
  transaction_requester->milestones = NULL;
  transaction_requester->transactions = NULL;
  rw_lock_handle_init(&transaction_requester->lock);

  return RC_OK;
}

retcode_t requester_destroy(transaction_requester_t *const transaction_requester) {
  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  } else if (transaction_requester->running) {
    return RC_STILL_RUNNING;
  }

  hash243_set_free(&transaction_requester->milestones);
  hash243_set_free(&transaction_requester->transactions);
  transaction_requester->node = NULL;
  rw_lock_handle_destroy(&transaction_requester->lock);
  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t requester_get_requested_transactions(transaction_requester_t *const transaction_requester,
                                               hash243_set_t *const transactions) {
  retcode_t ret = RC_OK;

  if (transaction_requester == NULL || transactions == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&transaction_requester->lock);

  if ((ret = hash243_set_append(&transaction_requester->transactions, transactions)) != RC_OK) {
    goto done;
  }
  if ((ret = hash243_set_append(&transaction_requester->milestones, transactions)) != RC_OK) {
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
  size = hash243_set_size(&transaction_requester->transactions) + hash243_set_size(&transaction_requester->milestones);
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

  return size >= transaction_requester->node->conf.requester_queue_size;
}

retcode_t requester_clear_request(transaction_requester_t *const transaction_requester, flex_trit_t const *const hash) {
  if (transaction_requester == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&transaction_requester->lock);
  hash243_set_remove(&transaction_requester->milestones, hash);
  hash243_set_remove(&transaction_requester->transactions, hash);
  rw_lock_handle_unlock(&transaction_requester->lock);

  return RC_OK;
}

retcode_t request_transaction(transaction_requester_t *const transaction_requester, tangle_t *const tangle,
                              flex_trit_t const *const hash, bool const is_milestone) {
  retcode_t ret = RC_OK;
  bool exists = false;

  if (transaction_requester == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  if (flex_trits_are_null(hash, FLEX_TRIT_SIZE_243)) {
    return RC_OK;
  }

  if ((ret = iota_tangle_transaction_exist(tangle, TRANSACTION_FIELD_HASH, hash, &exists)) != RC_OK) {
    return ret;
  }
  if (exists) {
    return RC_OK;
  }

  rw_lock_handle_wrlock(&transaction_requester->lock);

  if (is_milestone) {
    hash243_set_remove(&transaction_requester->transactions, hash);
    if ((ret = hash243_set_add(&transaction_requester->milestones, hash)) != RC_OK) {
      goto done;
    }
  } else if (!hash243_set_contains(&transaction_requester->milestones, hash) &&
             hash243_set_size(&transaction_requester->transactions) <
                 transaction_requester->node->conf.requester_queue_size) {
    if ((ret = hash243_set_add(&transaction_requester->transactions, hash)) != RC_OK) {
      goto done;
    }
  }

done:
  rw_lock_handle_unlock(&transaction_requester->lock);
  return ret;
}

retcode_t get_transaction_to_request(transaction_requester_t *const transaction_requester, tangle_t *const tangle,
                                     flex_trit_t *const hash, bool const milestone) {
  retcode_t ret = RC_OK;
  hash243_set_t *request_set = NULL;
  hash243_set_t *backup_set = NULL;
  hash243_set_entry_t *iter = NULL;
  hash243_set_entry_t *tmp = NULL;

  bool exists = false;

  if (transaction_requester == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  request_set = milestone ? &transaction_requester->milestones : &transaction_requester->transactions;
  backup_set = milestone ? &transaction_requester->transactions : &transaction_requester->milestones;

  rw_lock_handle_wrlock(&transaction_requester->lock);

  request_set = hash243_set_size(request_set) != 0 ? request_set : backup_set;

  HASH_ITER(hh, *request_set, iter, tmp) {
    memcpy(hash, iter->hash, FLEX_TRIT_SIZE_243);
    if ((ret = iota_tangle_transaction_exist(tangle, TRANSACTION_FIELD_HASH, iter->hash, &exists)) != RC_OK) {
      goto done;
    }
    hash243_set_remove_entry(request_set, iter);
    if (!exists) {
      hash243_set_add(request_set, hash);
      break;
    }
  }

  if (hash243_set_size(request_set) == 0) {
    memset(hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }

  if (rand_handle_probability() < transaction_requester->node->conf.p_remove_request &&
      request_set != &transaction_requester->milestones) {
    hash243_set_remove(request_set, hash);
  }

done:
  rw_lock_handle_unlock(&transaction_requester->lock);
  return ret;
}
