/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/snapshot/local_snapshots/pruning_manager.h"
#include <inttypes.h>
#include "ciri/consensus/snapshot/snapshots_provider.h"
#include "ciri/consensus/tangle/traversal.h"
#include "common/errors.h"
#include "common/model/milestone.h"
#include "utils/logger_helper.h"

#define PRUNING_MANAGER_LOGGER_ID "pruning_manager"

static logger_id_t logger_id;

static retcode_t iota_local_snapshots_pruning_manager_prune_transactions(pruning_manager_t *pm, tangle_t *const tangle,
                                                                         spent_addresses_provider_t *const sap);

typedef struct collect_transactions_for_pruning_do_func_params_s {
  uint64_t current_snapshot_index;
  tangle_t const *tangle;
  hash243_set_t transactions_to_prune;
} collect_transactions_for_pruning_do_func_params_t;

static retcode_t collect_unconfirmed_future_transactions_for_pruning_do_func(flex_trit_t *hash, iota_stor_pack_t *pack,
                                                                             void *data, bool *should_branch,
                                                                             bool *should_stop) {
  retcode_t ret = RC_OK;
  *should_stop = false;
  *should_branch = true;
  collect_transactions_for_pruning_do_func_params_t *params = data;

  if (pack->num_loaded == 0) {
    *should_branch = false;
    return RC_OK;
  }

  transaction_snapshot_index((iota_transaction_t *)pack->models[0]);

  if (transaction_snapshot_index((iota_transaction_t *)pack->models[0]) == 0) {
    if ((ret = hash243_set_add(&params->transactions_to_prune, hash)) != RC_OK) {
      return ret;
    }
    *should_branch = true;
  }

  return RC_OK;
}

static retcode_t collect_transactions_for_pruning_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data,
                                                          bool *should_branch, bool *should_stop) {
  retcode_t ret = RC_OK;
  *should_stop = false;
  *should_branch = false;
  collect_transactions_for_pruning_do_func_params_t *params = data;

  if (pack->num_loaded == 0) {
    return RC_OK;
  }

  uint64_t current_snapshot_index = transaction_snapshot_index((iota_transaction_t *)pack->models[0]);

  if (current_snapshot_index >= params->current_snapshot_index) {
    *should_branch = true;
    if ((ret = hash243_set_add(&params->transactions_to_prune, hash)) != RC_OK) {
      return ret;
    }

    ERR_BIND_RETURN(
        tangle_traversal_dfs_to_future(params->tangle, collect_unconfirmed_future_transactions_for_pruning_do_func,
                                       hash, NULL, &params),
        ret);
  }

cleanup:
  if (ret) {
    *should_stop = true;
    return ret;
  }

  return RC_OK;
}

static void *pruning_manager_routine(void *arg) {
  pruning_manager_t *pm = (pruning_manager_t *)arg;
  tangle_t tangle;

  {
    connection_config_t db_conf = {.db_path = pm->conf->tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Failed in initializing db\n");
      return NULL;
    }
  }

  spent_addresses_provider_t sap;
  {
    connection_config_t db_conf = {.db_path = pm->spent_addresses_service->conf->spent_addresses_db_path};

    if (pm->spent_addresses_service->conf->spent_addresses_files == NULL) {
      return RC_OK;
    }

    if (iota_spent_addresses_provider_init(&sap, &db_conf) != RC_OK) {
      log_error(logger_id, "Initializing spent addresses database connection failed\n");
      goto cleanup;
    }
  }

  lock_handle_lock(&pm->rw_lock);
  while (pm->running) {
    if (iota_local_snapshots_pruning_manager_prune_transactions(pm, &tangle, &sap) != RC_OK) {
      goto cleanup;
    }
    cond_handle_wait(&pm->cond_pruning_manager, &pm->rw_lock);
  }

cleanup:

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  if (iota_spent_addresses_provider_destroy(&sap) != RC_OK) {
    log_critical(logger_id, "Destroying spent addresses provider failed\n");
  }

  return NULL;
}

static retcode_t iota_local_snapshots_pruning_manager_prune_transactions(pruning_manager_t *pm, tangle_t *const tangle,
                                                                         spent_addresses_provider_t *const sap) {
  retcode_t err;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, milestone_pack);
  DECLARE_PACK_SINGLE_TX(tx, txp, tx_pack);
  collect_transactions_for_pruning_do_func_params_t params;
  bool can_prune_snapshot_index_entirely = true;

  hash243_set_entry_t *iter = NULL, *tmp = NULL;

  bool spent;

  while (pm->last_pruned_snapshot_index < pm->last_snapshot_index_to_prune) {
    params.current_snapshot_index = pm->last_pruned_snapshot_index + 1;
    params.tangle = &tangle;
    params.transactions_to_prune = NULL;
    ERR_BIND_GOTO(iota_tangle_milestone_load_by_index(&tangle, pm->last_pruned_snapshot_index, &milestone_pack), err,
                  cleanup);
    ERR_BIND_GOTO(tangle_traversal_dfs_to_past(&tangle, collect_transactions_for_pruning_do_func, milestone.hash,
                                               pm->conf->genesis_hash, NULL, &params),
                  err, cleanup);

    can_prune_snapshot_index_entirely = true;
    iter = NULL;
    tmp = NULL;
    bool spent;

    HASH_ITER(hh, params.transactions_to_prune, iter, tmp) {
      if (iota_snapshot_has_solid_entry_point(pm->new_snapshot, iter->hash)) {
        can_prune_snapshot_index_entirely = false;
        break;
      }
      hash_pack_reset(&tx_pack);
      ERR_BIND_GOTO(
          iota_tangle_transaction_load_partial(&tangle, iter->hash, &tx_pack, PARTIAL_TX_MODEL_ESSENCE_METADATA), err,
          cleanup);
      ERR_BIND_GOTO(iota_spent_addresses_service_was_tx_spent_from(sap, &tangle, &tx, iter->hash, &spent), err,
                    cleanup);
      ERR_BIND_GOTO(tips_cache_remove(pm->tips_cache, iter->hash), err, cleanup);
    }

    if (!can_prune_snapshot_index_entirely) {
      // Wait for next snapshot, i.e: updated solid entry points
      break;
    }

    hash243_set_add(&params.transactions_to_prune, milestone.hash);
    ERR_BIND_GOTO(iota_tangle_transaction_delete_batch(&tangle, params.transactions_to_prune), err, cleanup);
    ERR_BIND_GOTO(iota_tangle_milestone_delete(&tangle, milestone.hash), err, cleanup);
    log_info(logger_id, "Snapshot index % " PRId64 " was pruned successfully\n", pm->last_pruned_snapshot_index);
    pm->last_pruned_snapshot_index++;

    hash243_set_free(&params.transactions_to_prune);
  }

cleanup:

  lock_handle_unlock(&pm->rw_lock);

  if (err != RC_OK) {
    log_warning(logger_id, "Local snapshots pruning has failed with error code: %d\n", err);
  }
}

retcode_t iota_local_snapshots_pruning_manager_init(pruning_manager_t *pm,
                                                    snapshots_provider_t *const snapshot_provider,
                                                    spent_addresses_service_t const *spent_addresses_service,
                                                    tips_cache_t *const tips_cache, iota_consensus_conf_t *const conf) {
  logger_id = logger_helper_enable(PRUNING_MANAGER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(pm, 0, sizeof(pruning_manager_t));
  pm->conf = conf;
  pm->last_pruned_snapshot_index = snapshot_provider->inital_snapshot.metadata.index;
  pm->last_snapshot_index_to_prune = pm->last_pruned_snapshot_index;
  pm->snapshot_provider = snapshot_provider;
  pm->spent_addresses_service = spent_addresses_service;
  pm->tips_cache = tips_cache;
  rw_lock_handle_init(&pm->rw_lock);
  return RC_OK;
}

retcode_t iota_local_snapshots_pruning_manager_start(pruning_manager_t *const pm) {
  if (pm == NULL) {
    return RC_NULL_PARAM;
  }

  pm->running = true;

  log_info(logger_id, "Spawning pruning manager thread\n");
  if (thread_handle_create(&pm->pruning_manager_thread, (thread_routine_t)pruning_manager_routine, pm) != 0) {
    log_critical(logger_id, "Spawning pruning manager thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t iota_local_snapshots_pruning_manager_stop(pruning_manager_t *const pm) {
  retcode_t ret = RC_OK;

  if (pm == NULL) {
    return RC_NULL_PARAM;
  } else if (pm->running == false) {
    return RC_OK;
  }

  pm->running = false;
  cond_handle_signal(&pm->cond_pruning_manager);
  log_info(logger_id, "Shutting down pruning manager thread\n");
  if (thread_handle_join(pm->pruning_manager_thread, NULL) != 0) {
    log_error(logger_id, "Shutting down pruning manager thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  return ret;
}

retcode_t iota_local_snapshots_pruning_manager_destroy(pruning_manager_t *const pm) {
  retcode_t ret = RC_OK;

  if (pm == NULL) {
    return RC_NULL_PARAM;
  } else if (pm->running) {
    return RC_STILL_RUNNING;
  }

  cond_handle_destroy(&pm->cond_pruning_manager);
  memset(pm, 0, sizeof(pruning_manager_t));
  logger_helper_release(logger_id);
  rw_lock_handle_destroy(&pm->rw_lock);

  return ret;
}

void iota_local_snapshots_pruning_manager_update_current_snapshot(pruning_manager_t *const pm,
                                                                  snapshot_t const *const snapshot) {
  rw_lock_handle_wrlock(&pm->rw_lock);
  pm->last_snapshot_index_to_prune = snapshot->metadata.index;
  rw_lock_handle_unlock(&pm->rw_lock);

  pm->new_snapshot = snapshot;
  cond_handle_signal(&pm->cond_pruning_manager);
}
