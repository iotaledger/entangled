/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/pipeline/transaction_requester.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/node/node.h"
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
  transaction_requester->hashes = NULL;
  rw_lock_handle_init(&transaction_requester->lock);
  cond_handle_init(&transaction_requester->cond);

  return RC_OK;
}

retcode_t requester_destroy(transaction_requester_t *const transaction_requester) {
  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  } else if (transaction_requester->running) {
    return RC_STILL_RUNNING;
  }

  hash243_set_free(&transaction_requester->hashes);
  transaction_requester->node = NULL;
  rw_lock_handle_destroy(&transaction_requester->lock);
  cond_handle_destroy(&transaction_requester->cond);
  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t requester_get_requested_transactions(transaction_requester_t *const transaction_requester,
                                               hash243_stack_t *const hashes) {
  retcode_t ret = RC_OK;
  hash243_set_entry_t *iter = NULL;
  hash243_set_entry_t *tmp = NULL;

  if (transaction_requester == NULL || hashes == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&transaction_requester->lock);

  HASH_SET_ITER(transaction_requester->hashes, iter, tmp) {
    if ((ret = hash243_stack_push(hashes, iter->hash)) != RC_OK) {
      break;
    }
  }

  rw_lock_handle_unlock(&transaction_requester->lock);

  return ret;
}

size_t requester_size(transaction_requester_t *const transaction_requester) {
  size_t size = 0;

  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&transaction_requester->lock);
  size = hash243_set_size(transaction_requester->hashes);
  rw_lock_handle_unlock(&transaction_requester->lock);

  return size;
}

bool requester_is_full(transaction_requester_t *const transaction_requester) {
  size_t size = 0;

  if (transaction_requester == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&transaction_requester->lock);
  size = hash243_set_size(transaction_requester->hashes);
  rw_lock_handle_unlock(&transaction_requester->lock);

  return size >= transaction_requester->node->conf.requester_queue_size;
}

retcode_t requester_clear_request(transaction_requester_t *const transaction_requester, flex_trit_t const *const hash) {
  if (transaction_requester == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&transaction_requester->lock);
  hash243_set_remove(&transaction_requester->hashes, hash);
  rw_lock_handle_unlock(&transaction_requester->lock);

  return RC_OK;
}

retcode_t requester_is_requested(transaction_requester_t *const transaction_requester, flex_trit_t const *const hash,
                                 bool *const is_requested) {
  if (transaction_requester == NULL || hash == NULL || is_requested == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&transaction_requester->lock);
  *is_requested = hash243_set_contains(transaction_requester->hashes, hash);
  rw_lock_handle_unlock(&transaction_requester->lock);

  return RC_OK;
}

retcode_t request_transaction(transaction_requester_t *const transaction_requester, tangle_t *const tangle,
                              flex_trit_t const *const hash) {
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

  if (hash243_set_size(transaction_requester->hashes) >= transaction_requester->node->conf.requester_queue_size) {
    if ((ret = hash243_set_remove_entry(&transaction_requester->hashes, transaction_requester->hashes)) != RC_OK) {
      goto done;
    }
  }
  if ((ret = hash243_set_add(&transaction_requester->hashes, hash)) != RC_OK) {
    goto done;
  }

done:
  rw_lock_handle_unlock(&transaction_requester->lock);

  return ret;
}

retcode_t get_transaction_to_request(transaction_requester_t *const transaction_requester, tangle_t const *const tangle,
                                     flex_trit_t *const hash) {
  if (transaction_requester == NULL || tangle == NULL || hash == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_wrlock(&transaction_requester->lock);

  if (transaction_requester->hashes != NULL) {
    memcpy(hash, transaction_requester->hashes->hash, FLEX_TRIT_SIZE_243);
    hash243_set_remove_entry(&transaction_requester->hashes, transaction_requester->hashes);
  } else {
    memset(hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }

  rw_lock_handle_unlock(&transaction_requester->lock);

  return RC_OK;
}
