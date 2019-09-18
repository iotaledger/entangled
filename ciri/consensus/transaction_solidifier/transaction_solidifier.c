/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/transaction_solidifier/transaction_solidifier.h"
#include "ciri/consensus/tangle/traversal.h"
#include "utils/logger_helper.h"

#define TRANSACTION_SOLIDIFIER_LOGGER_ID "transaction_solidifier"
#define SOLID_PROPAGATION_INTERVAL_MS 500uLL

static logger_id_t logger_id;

/*
 * Forward declarations
 */

static retcode_t check_approvee_solid_state(transaction_solidifier_t *const ts, tangle_t *const tangle,
                                            flex_trit_t *const approvee, bool *solid);

static retcode_t check_transaction_and_update_solid_state(transaction_solidifier_t *const ts, tangle_t *const tangle,
                                                          flex_trit_t *const transaction, bool *const is_new_solid);

static retcode_t add_new_solid_transaction(transaction_solidifier_t *const ts, flex_trit_t *const hash);

static retcode_t check_solidity_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data, bool *should_branch,
                                        bool *should_stop);

/*
 * Private functions
 */

typedef struct check_solidity_do_func_params_s {
  transaction_solidifier_t *ts;
  tangle_t *tangle;
  bool is_solid;
  hash243_set_t *solid_transactions_candidates;
  hash243_set_t *solid_entry_points;
  int max_analyzed;
} check_solidity_do_func_params_t;

static retcode_t propagate_solid_transactions(transaction_solidifier_t *const ts, tangle_t *const tangle) {
  retcode_t ret = RC_OK;
  hash243_set_t transactions_to_propagate = NULL;
  hash243_set_entry_t *curr_entry = NULL;
  hash243_set_entry_t *tmp_entry = NULL;
  flex_trit_t *approver_hash = NULL;
  iota_stor_pack_t hash_pack;

  lock_handle_lock(&ts->lock);
  transactions_to_propagate = ts->newly_set_solid_transactions;
  ts->newly_set_solid_transactions = NULL;
  lock_handle_unlock(&ts->lock);

  if ((ret = hash_pack_init(&hash_pack, 32)) != RC_OK) {
    goto done;
  }

  HASH_ITER(hh, transactions_to_propagate, curr_entry, tmp_entry) {
    if (!ts->running) {
      break;
    }

    hash_pack_reset(&hash_pack);
    if ((ret = iota_tangle_transaction_load_hashes_of_approvers(tangle, curr_entry->hash, &hash_pack, 0)) != RC_OK) {
      log_error(logger_id,
                "Loading hashes of approvers while propagating "
                "solidity failed\n");
      goto done;
    }

    for (size_t approver_index = 0; approver_index < hash_pack.num_loaded; ++approver_index) {
      approver_hash = ((flex_trit_t *)hash_pack.models[approver_index]);
      if (hash243_set_contains(transactions_to_propagate, approver_hash)) {
        continue;
      }
      if ((ret = iota_consensus_transaction_solidifier_check_and_update_solid_state(ts, tangle, approver_hash)) !=
          RC_OK) {
        log_error(logger_id,
                  "Checking and updating solid state while propagating "
                  "solidity failed\n");
        goto done;
      }
    }
  }

done:
  hash243_set_free(&transactions_to_propagate);
  hash_pack_free(&hash_pack);
  return ret;
}

static void *spawn_solid_transactions_propagation(void *arg) {
  transaction_solidifier_t *ts = (transaction_solidifier_t *)arg;
  tangle_t tangle;

  {
    storage_connection_config_t db_conf = {.db_path = ts->conf->tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Initializing tangle connection failed\n");
      return NULL;
    }
  }

  lock_handle_t lock_cond;
  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (ts->running) {
    while (hash243_set_size(ts->newly_set_solid_transactions) > 0) {
      if (propagate_solid_transactions(ts, &tangle) != RC_OK) {
        log_error(logger_id, "Solid transaction propagation failed\n");
      }
    }
    cond_handle_timedwait(&ts->cond, &lock_cond, SOLID_PROPAGATION_INTERVAL_MS);
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

retcode_t iota_consensus_transaction_solidifier_init(transaction_solidifier_t *const ts,
                                                     iota_consensus_conf_t *const conf,
                                                     transaction_requester_t *const transaction_requester,
                                                     snapshots_provider_t *const snapshots_provider,
                                                     tips_cache_t *const tips) {
  ts->conf = conf;
  ts->transaction_requester = transaction_requester;
  ts->running = false;
  ts->newly_set_solid_transactions = NULL;
  ts->snapshots_provider = snapshots_provider;
  ts->tips = tips;
  lock_handle_init(&ts->lock);
  cond_handle_init(&ts->cond);
  logger_id = logger_helper_enable(TRANSACTION_SOLIDIFIER_LOGGER_ID, LOGGER_DEBUG, true);
  return RC_OK;
}

retcode_t iota_consensus_transaction_solidifier_start(transaction_solidifier_t *const ts) {
  if (ts == NULL) {
    return RC_NULL_PARAM;
  }

  ts->running = true;
  log_info(logger_id, "Spawning transaction solidifier thread\n");
  if (thread_handle_create(&ts->thread, (thread_routine_t)spawn_solid_transactions_propagation, ts) != 0) {
    log_critical(logger_id, "Spawning transaction solidifier thread failed\n");
    return RC_THREAD_CREATE;
  }
  return RC_OK;
}

retcode_t iota_consensus_transaction_solidifier_stop(transaction_solidifier_t *const ts) {
  retcode_t ret = RC_OK;

  if (ts == NULL) {
    return RC_NULL_PARAM;
  } else if (ts->running == false) {
    return RC_OK;
  }

  log_info(logger_id, "Shutting down transaction solidifier thread\n");
  ts->running = false;
  cond_handle_signal(&ts->cond);
  if (thread_handle_join(ts->thread, NULL) != 0) {
    log_error(logger_id, "Shutting down transaction solidifier thread failed\n");
    ret = RC_THREAD_JOIN;
  }
  return ret;
}

retcode_t iota_consensus_transaction_solidifier_destroy(transaction_solidifier_t *const ts) {
  if (ts == NULL) {
    return RC_NULL_PARAM;
  } else if (ts->running) {
    return RC_STILL_RUNNING;
  }

  if (ts->newly_set_solid_transactions) {
    hash243_set_free(&ts->newly_set_solid_transactions);
  }
  ts->transaction_requester = NULL;
  ts->newly_set_solid_transactions = NULL;
  ts->conf = NULL;

  lock_handle_destroy(&ts->lock);
  cond_handle_destroy(&ts->cond);

  logger_helper_release(logger_id);
  return RC_OK;
}

static retcode_t check_solidity_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data, bool *should_branch,
                                        bool *should_stop) {
  *should_stop = false;
  *should_branch = false;
  check_solidity_do_func_params_t *params = data;
  transaction_solidifier_t *ts = params->ts;
  tangle_t *tangle = params->tangle;

  if (params->max_analyzed == 0) {
    *should_stop = true;
    params->is_solid = false;
    return RC_OK;
  }
  params->max_analyzed--;

  // Transaction is not marked solid, but it is a candidate
  if (pack->num_loaded == 1 && !((transaction_solid((iota_transaction_t *)pack->models[0])))) {
    *should_branch = true;
    return hash243_set_add(params->solid_transactions_candidates, hash);
  } else if (pack->num_loaded == 0 && !hash243_set_contains(*params->solid_entry_points, hash)) {
    params->is_solid = false;
    return request_transaction(ts->transaction_requester, tangle, hash);
  }

  return RC_OK;
}

retcode_t iota_consensus_transaction_solidifier_check_solidity(transaction_solidifier_t *const ts,
                                                               tangle_t *const tangle, flex_trit_t *const hash,
                                                               int max_analyzed, bool *const is_solid) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);
  hash243_set_t solid_transactions_candidates = NULL;
  hash243_set_t analyzed_hashes = NULL;
  hash243_set_t solid_entry_points_hashes = NULL;

  ret = iota_tangle_transaction_load_partial(tangle, hash, &pack, PARTIAL_TX_MODEL_ESSENCE_ATTACHMENT_METADATA);
  if (ret != RC_OK) {
    log_error(logger_id, "No transactions were loaded for the provided hash\n");
    return ret;
  }

  if (transaction_solid(curr_tx)) {
    *is_solid = true;
    return RC_OK;
  }

  iota_snapshot_solid_entry_points_set(&ts->snapshots_provider->initial_snapshot, &analyzed_hashes);
  hash243_set_append(&analyzed_hashes, &solid_entry_points_hashes);

  max_analyzed += hash243_set_size(solid_entry_points_hashes);

  check_solidity_do_func_params_t params = {.ts = ts,
                                            .tangle = tangle,
                                            .is_solid = true,
                                            .solid_transactions_candidates = &solid_transactions_candidates,
                                            .solid_entry_points = &solid_entry_points_hashes,
                                            .max_analyzed = max_analyzed};

  if ((ret = tangle_traversal_dfs_to_past(tangle, check_solidity_do_func, hash, ts->conf->genesis_hash,
                                          &analyzed_hashes, &params)) != RC_OK) {
    *is_solid = false;
    goto done;
  }

  if (params.is_solid) {
    *is_solid = true;
    log_debug(logger_id, "In %s, updating solid state\n", __FUNCTION__);

    ret = iota_tangle_transactions_update_solidity(tangle, solid_transactions_candidates, true);

    lock_handle_lock(&ts->lock);
    hash243_set_append(&solid_transactions_candidates, &ts->newly_set_solid_transactions);
    lock_handle_unlock(&ts->lock);
  }

done:
  hash243_set_free(&solid_transactions_candidates);
  hash243_set_free(&analyzed_hashes);
  hash243_set_free(&solid_entry_points_hashes);
  return ret;
}

static retcode_t check_transaction_and_update_solid_state(transaction_solidifier_t *const ts, tangle_t *const tangle,
                                                          flex_trit_t *const hash, bool *const is_new_solid) {
  retcode_t ret = RC_OK;
  bool is_trunk_solid = false;
  bool is_branch_solid = false;
  DECLARE_PACK_SINGLE_TX(transaction_s, transaction, pack);

  *is_new_solid = false;

  if (iota_snapshot_has_solid_entry_point(&ts->snapshots_provider->initial_snapshot, hash)) {
    return RC_OK;
  }

  ret = iota_tangle_transaction_load_partial(tangle, hash, &pack, PARTIAL_TX_MODEL_ESSENCE_ATTACHMENT_METADATA);
  if (ret != RC_OK || pack.num_loaded == 0) {
    log_error(logger_id, "No transactions were loaded for the provided hash\n");
    return ret;
  }

  if (!transaction_solid(transaction)) {
    if ((ret = check_approvee_solid_state(ts, tangle, transaction_trunk(transaction), &is_trunk_solid)) != RC_OK) {
      *is_new_solid = false;
      log_error(logger_id, "Checking solidity of trunk failed\n");
      return ret;
    }

    if ((ret = check_approvee_solid_state(ts, tangle, transaction_branch(transaction), &is_branch_solid)) != RC_OK) {
      *is_new_solid = false;
      log_error(logger_id, "Checking solidity of branch failed\n");
      return ret;
    }

    if ((*is_new_solid = is_trunk_solid && is_branch_solid)) {
      if ((ret = iota_tangle_transaction_update_solidity(tangle, hash, true)) != RC_OK) {
        log_error(logger_id, "Updating solid state failed\n");
        return ret;
      }
    }
  }

  return ret;
}

static retcode_t check_approvee_solid_state(transaction_solidifier_t *const ts, tangle_t *const tangle,
                                            flex_trit_t *const approvee, bool *solid) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);

  if (iota_snapshot_has_solid_entry_point(&ts->snapshots_provider->initial_snapshot, approvee)) {
    *solid = true;
    return RC_OK;
  }

  if ((ret = iota_tangle_transaction_load_partial(tangle, approvee, &pack, PARTIAL_TX_MODEL_METADATA)) != RC_OK) {
    log_error(logger_id,
              "Loading transaction metadata for checking approvee solid state "
              "failed\n");
    *solid = false;
    return ret;
  }

  if (pack.num_loaded == 0) {
    *solid = false;
    return ret;
  }

  if (memcmp(approvee, ts->conf->genesis_hash, FLEX_TRIT_SIZE_243) == 0) {
    *solid = true;
    return ret;
  }

  *solid = transaction_solid(curr_tx);

  return ret;
}

retcode_t iota_consensus_transaction_solidifier_check_and_update_solid_state(transaction_solidifier_t *const ts,
                                                                             tangle_t *const tangle,
                                                                             flex_trit_t *const hash) {
  retcode_t ret = RC_OK;
  bool is_new_solid = false;

  if (ts->transaction_requester == NULL) {
    return RC_OK;
  }

  if ((ret = check_transaction_and_update_solid_state(ts, tangle, hash, &is_new_solid))) {
    log_error(logger_id, "In %s, failed check_transaction_and_update_solid_state\n", __FUNCTION__);
    return ret;
  }

  if (is_new_solid) {
    return add_new_solid_transaction(ts, hash);
  }

  return ret;
}

static retcode_t add_new_solid_transaction(transaction_solidifier_t *const ts, flex_trit_t *const hash) {
  retcode_t ret = RC_OK;

  lock_handle_lock(&ts->lock);
  ret = hash243_set_add(&ts->newly_set_solid_transactions, hash);
  lock_handle_unlock(&ts->lock);

  if (ret != RC_OK) {
    return ret;
  }

  return tips_cache_set_solid(ts->tips, hash);
}

retcode_t iota_consensus_transaction_solidifier_update_status(transaction_solidifier_t *const ts,
                                                              tangle_t *const tangle, iota_transaction_t *const tx) {
  retcode_t ret = RC_OK;
  uint64_t approvers_count = 0;

  if ((ret = requester_clear_request(ts->transaction_requester, transaction_hash(tx))) != RC_OK) {
    return ret;
  }

  if ((ret = iota_tangle_transaction_approvers_count(tangle, transaction_hash(tx), &approvers_count)) != RC_OK) {
    return ret;
  }

  if (approvers_count == 0) {
    if ((ret = tips_cache_add(ts->tips, transaction_hash(tx))) != RC_OK) {
      return ret;
    }
  }

  if ((ret = tips_cache_remove(ts->tips, transaction_trunk(tx))) != RC_OK ||
      (ret = tips_cache_remove(ts->tips, transaction_branch(tx))) != RC_OK) {
    return ret;
  }

  return iota_consensus_transaction_solidifier_check_and_update_solid_state(ts, tangle, transaction_hash(tx));
}
