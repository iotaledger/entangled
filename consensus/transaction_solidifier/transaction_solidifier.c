/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "transaction_solidifier.h"

#include "consensus/utils/tangle_traversals.h"
#include "utils/hash_maps.h"
#include "utils/logger_helper.h"

#define TRANSACTION_SOLIDIFIER_LOGGER_ID "transaction_solidifier"

#define SOLID_PROPAGATION_INTERVAL 500000uLL

/*
 * Forward declarations
 */

static retcode_t check_approvee_solid_state(transaction_solidifier_t *const ts,
                                            flex_trit_t *const approvee,
                                            bool *solid);

static retcode_t check_transaction_and_update_solid_state(
    transaction_solidifier_t *const ts, flex_trit_t *const transaction,
    bool *const is_new_solid);

static void *spawn_solid_transactions_propagation(void *arg);

static retcode_t add_new_solid_transaction(transaction_solidifier_t *const ts,
                                           flex_trit_t *const hash);

static retcode_t propagate_solid_transactions(
    transaction_solidifier_t *const ts);

static retcode_t check_solidity_do_func(flex_trit_t *hash,
                                        iota_stor_pack_t *pack, void *data,
                                        bool *should_branch, bool *should_stop);

/*
 * Auxiliary structs
 */

typedef struct check_solidity_do_func_params_s {
  transaction_solidifier_t *ts;
  bool is_milestone;
  bool is_solid;
} check_solidity_do_func_params_t;

static void *spawn_solid_transactions_propagation(void *arg) {
  transaction_solidifier_t *ts = (transaction_solidifier_t *)arg;
  while (ts->running) {
    if (propagate_solid_transactions(ts) != RC_OK) {
      log_critical(TRANSACTION_SOLIDIFIER_LOGGER_ID,
                   "Failed in solid transaction propagation %s\n");
    }
    usleep(SOLID_PROPAGATION_INTERVAL);
  }
  return NULL;
}

static retcode_t propagate_solid_transactions(
    transaction_solidifier_t *const ts) {
  retcode_t ret;
  hash243_set_t transactions_to_propagate = NULL;
  lock_handle_lock(&ts->lock);
  ret = hash243_set_append(&ts->newly_set_solid_transactions,
                           &transactions_to_propagate);
  hash243_set_free(&ts->newly_set_solid_transactions);
  lock_handle_unlock(&ts->lock);
  if (ret) {
    return ret;
  }

  iota_stor_pack_t hash_pack;
  if ((ret = hash_pack_init(&hash_pack, 10)) != RC_OK) {
    return ret;
  }

  hash243_set_entry_t *curr_entry = NULL;
  hash243_set_entry_t *tmp_entry = NULL;
  flex_trit_t *curr_approver_hash;
  uint16_t approver_index;
  HASH_ITER(hh, transactions_to_propagate, curr_entry, tmp_entry) {
    if (!ts->running) {
      break;
    }

    hash_pack_reset(&hash_pack);
    if ((ret = iota_tangle_transaction_load_hashes_of_approvers(
             ts->tangle, curr_entry->hash, &hash_pack))) {
      return ret;
    }
    for (approver_index = 0; approver_index < hash_pack.num_loaded;
         ++approver_index) {
      curr_approver_hash = (flex_trit_t *)hash_pack.models[approver_index];
      if ((ret =
               iota_consensus_transaction_solidifier_check_and_update_solid_state(
                   ts, curr_approver_hash)) != RC_OK) {
        log_error(TRANSACTION_SOLIDIFIER_LOGGER_ID,
                  "In %s, failed check_transaction_and_update_solid_state\n",
                  __FUNCTION__);
        return ret;
      }
    }
  }
  return RC_OK;
}

retcode_t iota_consensus_transaction_solidifier_init(
    transaction_solidifier_t *const ts, iota_consensus_conf_t *const conf,
    tangle_t *const tangle, requester_state_t *const requester,
    tips_cache_t *const tips) {
  ts->conf = conf;
  ts->tangle = tangle;
  ts->requester = requester;
  ts->running = false;
  ts->newly_set_solid_transactions = NULL;
  ts->solid_transactions_candidates = NULL;
  ts->tips = tips;
  lock_handle_init(&ts->lock);
  logger_helper_init(TRANSACTION_SOLIDIFIER_LOGGER_ID, LOGGER_DEBUG, true);
  return RC_OK;
}

retcode_t iota_consensus_transaction_solidifier_start(
    transaction_solidifier_t *const ts) {
  if (ts == NULL) {
    return RC_CONSENSUS_NULL_PTR;
  }

  ts->running = true;
  log_info(TRANSACTION_SOLIDIFIER_LOGGER_ID,
           "Spawning transaction solidifier thread\n");
  if (thread_handle_create(
          &ts->thread, (thread_routine_t)spawn_solid_transactions_propagation,
          ts) != 0) {
    log_critical(TRANSACTION_SOLIDIFIER_LOGGER_ID,
                 "Spawning transaction solidifier thread failed\n");
    return RC_API_FAILED_THREAD_SPAWN;
  }
  return RC_OK;
}

retcode_t iota_consensus_transaction_solidifier_stop(
    transaction_solidifier_t *const ts) {
  retcode_t ret = RC_OK;

  if (ts == NULL) {
    return RC_CONSENSUS_NULL_PTR;
  } else if (ts->running == false) {
    return RC_OK;
  }

  ts->running = false;

  log_info(TRANSACTION_SOLIDIFIER_LOGGER_ID,
           "Shutting down transaction solidifier thread\n");
  if (thread_handle_join(ts->thread, NULL) != 0) {
    log_error(TRANSACTION_SOLIDIFIER_LOGGER_ID,
              "Shutting down transaction solidifier thread failed\n");
    ret = RC_API_FAILED_THREAD_JOIN;
  }
  return ret;
}

retcode_t iota_consensus_transaction_solidifier_destroy(
    transaction_solidifier_t *const ts) {
  if (ts == NULL) {
    return RC_CONSENSUS_NULL_PTR;
  } else if (ts->running) {
    return RC_API_STILL_RUNNING;
  }

  ts->tangle = NULL;
  ts->requester = NULL;
  ts->newly_set_solid_transactions = NULL;
  ts->solid_transactions_candidates = NULL;
  ts->conf = NULL;

  lock_handle_destroy(&ts->lock);

  logger_helper_destroy(TRANSACTION_SOLIDIFIER_LOGGER_ID);
  return RC_OK;
}

static retcode_t check_solidity_do_func(flex_trit_t *hash,
                                        iota_stor_pack_t *pack, void *data,
                                        bool *should_branch,
                                        bool *should_stop) {
  *should_stop = false;
  *should_branch = false;
  check_solidity_do_func_params_t *params = data;
  transaction_solidifier_t *ts = params->ts;

  // Transaction is not marked solid, but it is a candidate
  if (pack->num_loaded == 1 && !((iota_transaction_t)pack->models[0])->solid) {
    *should_branch = true;
    return hash243_set_add(&ts->solid_transactions_candidates, hash);
  } else if (pack->num_loaded == 0) {
    if (memcmp(hash, ts->conf->genesis_hash, FLEX_TRIT_SIZE_243) != 0) {
      params->is_solid = false;
      return request_transaction(ts->requester, hash, params->is_milestone);
    }
  }

  return RC_OK;
}

retcode_t iota_consensus_transaction_solidifier_check_solidity(
    transaction_solidifier_t *const ts, flex_trit_t *const hash,
    bool is_milestone, bool *const is_solid) {
  retcode_t ret;

  trit_array_t hash_trits = {.trits = hash,
                             .num_trits = NUM_TRITS_HASH,
                             .num_bytes = FLEX_TRIT_SIZE_243,
                             .dynamic = 0};
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);

  ret = iota_tangle_transaction_load(ts->tangle, TRANSACTION_FIELD_HASH,
                                     &hash_trits, &pack);
  if (ret != RC_OK || pack.num_loaded == 0) {
    log_error(TRANSACTION_SOLIDIFIER_LOGGER_ID,
              "No transactions were loaded for the provided hash\n");
    *is_solid = false;
    return ret;
  }

  if (curr_tx->solid) {
    *is_solid = true;
    return RC_OK;
  }

  lock_handle_lock(&ts->lock);
  check_solidity_do_func_params_t params = {
      .ts = ts, .is_milestone = is_milestone, .is_solid = true};

  if ((ret = tangle_traversal_dfs_to_genesis(ts->tangle, check_solidity_do_func,
                                             hash, ts->conf->genesis_hash, NULL,
                                             &params)) != RC_OK) {
    *is_solid = false;
    goto done;
  }
  if (params.is_solid) {
    log_debug(TRANSACTION_SOLIDIFIER_LOGGER_ID, "In %s, updating solid state\n",
              __FUNCTION__);
    ret = iota_tangle_transactions_update_solid_state(
        ts->tangle, ts->solid_transactions_candidates, true);
    hash243_set_append(&ts->solid_transactions_candidates,
                       &ts->newly_set_solid_transactions);
  }
done:
  hash243_set_free(&ts->solid_transactions_candidates);
  lock_handle_unlock(&ts->lock);
  return ret;
}

static retcode_t check_transaction_and_update_solid_state(
    transaction_solidifier_t *const ts, flex_trit_t *const hash,
    bool *const is_new_solid) {
  retcode_t ret;

  *is_new_solid = false;

  DECLARE_PACK_SINGLE_TX(transaction_s, transaction, pack);
  trit_array_t hash_trits = {.trits = hash,
                             .num_trits = NUM_TRITS_HASH,
                             .num_bytes = FLEX_TRIT_SIZE_243,
                             .dynamic = 0};

  ret = iota_tangle_transaction_load(ts->tangle, TRANSACTION_FIELD_HASH,
                                     &hash_trits, &pack);
  if (ret != RC_OK || pack.num_loaded == 0) {
    log_error(TRANSACTION_SOLIDIFIER_LOGGER_ID,
              "No transactions were loaded for the provided hash\n");
    return ret;
  }

  if (!transaction->solid) {
    *is_new_solid = true;

    if ((ret = check_approvee_solid_state(ts, transaction->trunk,
                                          is_new_solid)) != RC_OK) {
      *is_new_solid = false;
      return ret;
    }

    if ((ret = check_approvee_solid_state(ts, transaction->branch,
                                          is_new_solid)) != RC_OK) {
      *is_new_solid = false;
      return ret;
    }

    if (*is_new_solid) {
      if ((ret = iota_tangle_transaction_update_solid_state(
               ts->tangle, transaction->hash, true)) != RC_OK) {
        return ret;
      }
    }
  }

  return ret;
}

static retcode_t check_approvee_solid_state(transaction_solidifier_t *const ts,
                                            flex_trit_t *const approvee,
                                            bool *solid) {
  retcode_t ret = RC_OK;

  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);

  trit_array_t approvee_trits = {.trits = approvee,
                                 .num_trits = HASH_LENGTH_TRIT,
                                 .num_bytes = FLEX_TRIT_SIZE_243,
                                 .dynamic = 0};

  ret = iota_tangle_transaction_load(ts->tangle, TRANSACTION_FIELD_HASH,
                                     &approvee_trits, &pack);
  if (ret != RC_OK || pack.num_loaded == 0) {
    *solid = false;
    return request_transaction(ts->requester, approvee, false);
  }
  if (memcmp(curr_tx_s.hash, ts->conf->genesis_hash, FLEX_TRIT_SIZE_243) == 0) {
    *solid = true;
    return ret;
  }
  *solid = curr_tx_s.solid;
  return RC_OK;
}

retcode_t iota_consensus_transaction_solidifier_check_and_update_solid_state(
    transaction_solidifier_t *const ts, flex_trit_t *const hash) {
  retcode_t ret;
  bool is_new_solid;

  if (ts->requester == NULL) {
    return RC_OK;
  }

  if ((ret =
           check_transaction_and_update_solid_state(ts, hash, &is_new_solid))) {
    log_debug(TRANSACTION_SOLIDIFIER_LOGGER_ID,
              "In %s, failed check_transaction_and_update_solid_state\n",
              __FUNCTION__);
    return ret;
  }

  if (is_new_solid) {
    return add_new_solid_transaction(ts, hash);
  }
  return RC_OK;
}

static retcode_t add_new_solid_transaction(transaction_solidifier_t *const ts,
                                           flex_trit_t *const hash) {
  retcode_t ret = RC_OK;

  lock_handle_lock(&ts->lock);

  if ((ret = hash243_set_add(&ts->newly_set_solid_transactions, hash)) !=
      RC_OK) {
    goto done;
  }

  ret = tips_cache_set_solid(ts->tips, hash);

done:
  lock_handle_unlock(&ts->lock);
  return ret;
}

retcode_t iota_consensus_transaction_solidifier_update_status(
    transaction_solidifier_t *const ts, iota_transaction_t const tx) {
  retcode_t ret = RC_OK;
  size_t approvers_count = 0;

  if ((ret = requester_clear_request(ts->requester, tx->hash)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_tangle_transaction_approvers_count(
           ts->tangle, tx->hash, &approvers_count)) != RC_OK) {
    return ret;
  }

  if (approvers_count == 0) {
    if ((ret = tips_cache_add(ts->tips, tx->hash)) != RC_OK) {
      return ret;
    }
  }

  if ((ret = tips_cache_remove(ts->tips, tx->trunk)) != RC_OK ||
      (ret = tips_cache_remove(ts->tips, tx->branch)) != RC_OK) {
    return ret;
  }

  return iota_consensus_transaction_solidifier_check_and_update_solid_state(
      ts, tx->hash);
}
