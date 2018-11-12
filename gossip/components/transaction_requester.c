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

#define REQUESTER_LOGGER_ID "requester"

retcode_t requester_init(requester_state_t *const state,
                         iota_gossip_conf_t *const conf,
                         tangle_t *const tangle) {
  if (state == NULL || tangle == NULL || conf == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  logger_helper_init(REQUESTER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(state, 0, sizeof(requester_state_t));
  state->conf = conf;
  state->milestones = NULL;
  state->transactions = NULL;
  state->tangle = tangle;
  rw_lock_handle_init(&state->lock);

  return RC_OK;
}

retcode_t requester_destroy(requester_state_t *const state) {
  if (state == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  hash243_set_free(&state->milestones);
  hash243_set_free(&state->transactions);
  state->tangle = NULL;
  rw_lock_handle_destroy(&state->lock);
  logger_helper_destroy(REQUESTER_LOGGER_ID);

  return RC_OK;
}

retcode_t requester_get_requested_transactions(
    requester_state_t *const state, hash243_set_t *const transactions) {
  retcode_t ret = RC_OK;

  if (state == NULL || transactions == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  rw_lock_handle_rdlock(&state->lock);

  if ((ret = hash243_set_append(&state->transactions, transactions)) != RC_OK) {
    goto done;
  }
  if ((ret = hash243_set_append(&state->milestones, transactions)) != RC_OK) {
    goto done;
  }

done:
  rw_lock_handle_unlock(&state->lock);
  return ret;
}

size_t requester_size(requester_state_t *const state) {
  size_t size = 0;

  if (state == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  rw_lock_handle_rdlock(&state->lock);
  size = hash243_set_size(&state->transactions) +
         hash243_set_size(&state->milestones);
  rw_lock_handle_unlock(&state->lock);

  return size;
}

bool requester_is_full(requester_state_t *const state) {
  size_t size = 0;

  if (state == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  rw_lock_handle_rdlock(&state->lock);
  size = hash243_set_size(&state->transactions);
  rw_lock_handle_unlock(&state->lock);

  return size >= state->conf->requester_queue_size;
}

retcode_t requester_clear_request(requester_state_t *const state,
                                  flex_trit_t const *const hash) {
  if (state == NULL || hash == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  rw_lock_handle_wrlock(&state->lock);
  hash243_set_remove(&state->milestones, hash);
  hash243_set_remove(&state->transactions, hash);
  rw_lock_handle_unlock(&state->lock);

  return RC_OK;
}

retcode_t request_transaction(requester_state_t *const state,
                              flex_trit_t *const hash,
                              bool const is_milestone) {
  retcode_t ret = RC_OK;
  bool exists = false;
  trit_array_t const key = {.trits = hash,
                            .num_trits = HASH_LENGTH_TRIT,
                            .num_bytes = FLEX_TRIT_SIZE_243,
                            .dynamic = 0};

  if (state == NULL || hash == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  if (flex_trits_are_null(hash, FLEX_TRIT_SIZE_243)) {
    return RC_OK;
  }

  if ((ret = iota_tangle_transaction_exist(
           state->tangle, TRANSACTION_FIELD_HASH, &key, &exists)) != RC_OK) {
    return ret;
  }
  if (exists) {
    return RC_OK;
  }

  rw_lock_handle_wrlock(&state->lock);

  if (is_milestone) {
    hash243_set_remove(&state->transactions, hash);
    if ((ret = hash243_set_add(&state->milestones, hash)) != RC_OK) {
      goto done;
    }
  } else if (!hash243_set_contains(&state->milestones, hash) &&
             hash243_set_size(&state->transactions) <
                 state->conf->requester_queue_size) {
    if ((ret = hash243_set_add(&state->transactions, hash)) != RC_OK) {
      goto done;
    }
  }

done:
  rw_lock_handle_unlock(&state->lock);
  return ret;
}

retcode_t get_transaction_to_request(requester_state_t *const state,
                                     flex_trit_t *const hash,
                                     bool const milestone) {
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

  if (state == NULL || hash == NULL) {
    return RC_REQUESTER_COMPONENT_NULL_STATE;
  }

  request_set = milestone ? &state->milestones : &state->transactions;
  backup_set = milestone ? &state->transactions : &state->milestones;

  rw_lock_handle_wrlock(&state->lock);

  request_set = hash243_set_size(request_set) != 0 ? request_set : backup_set;

  HASH_ITER(hh, *request_set, iter, tmp) {
    key.trits = iter->hash;
    if ((ret = iota_tangle_transaction_exist(
             state->tangle, TRANSACTION_FIELD_HASH, &key, &exists)) != RC_OK) {
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

  if (rand_handle_probability() < state->conf->p_remove_request &&
      request_set != &state->milestones) {
    hash243_set_remove(request_set, iter->hash);
  }

done:
  rw_lock_handle_unlock(&state->lock);
  return ret;
}
