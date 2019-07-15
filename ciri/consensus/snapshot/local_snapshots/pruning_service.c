/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/snapshot/local_snapshots/pruning_service.h"
#include <inttypes.h>
#include "ciri/consensus/snapshot/snapshots_provider.h"
#include "ciri/consensus/tangle/traversal.h"
#include "common/errors.h"
#include "common/model/milestone.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"
#include "utils/time.h"

#define PRUNING_RESCAN_INTERVAL_SHORT_MS 1ULL
#define PRUNING_RESCAN_INTERVAL_MS 1000ULL

#define PRUNING_SERVICE_LOGGER_ID "pruning_service"

static logger_id_t logger_id;

static retcode_t prune_transactions(pruning_service_t *const ps, tangle_t const *const tangle,
                                    spent_addresses_provider_t *const sap);

static retcode_t collect_transactions_to_prune(pruning_service_t *const ps, tangle_t const *const tangle,
                                               spent_addresses_provider_t *const sap,
                                               flex_trit_t const *const entry_point_hash,
                                               hash243_set_t *const transactions_to_prune);

static retcode_t collect_unconfirmed_future_transactions_for_pruning_do_func(flex_trit_t *hash, iota_stor_pack_t *pack,
                                                                             void *data, bool *should_branch,
                                                                             bool *should_stop);

static retcode_t collect_transactions_for_pruning_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data,
                                                          bool *should_branch, bool *should_stop);

static void *pruning_service_routine(void *arg);

typedef struct collect_transactions_for_pruning_do_func_params_s {
  uint64_t current_snapshot_index;
  tangle_t const *const tangle;
  hash243_set_t *const transactions_to_prune;
} collect_transactions_for_pruning_do_func_params_t;

static retcode_t collect_unconfirmed_future_transactions_for_pruning_do_func(flex_trit_t *hash, iota_stor_pack_t *pack,
                                                                             void *data, bool *should_branch,
                                                                             bool *should_stop) {
  retcode_t ret = RC_OK;
  *should_stop = false;
  *should_branch = false;

  if (pack->num_loaded == 0) {
    return RC_OK;
  }

  if (transaction_snapshot_index((iota_transaction_t *)pack->models[0]) == 0) {
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
    ERR_BIND_RETURN(
        tangle_traversal_dfs_to_future(params->tangle, collect_unconfirmed_future_transactions_for_pruning_do_func,
                                       hash, params->transactions_to_prune, &params),
        ret);
  }

  if (ret) {
    *should_stop = true;
    return ret;
  }

  return RC_OK;
}

static void *pruning_service_routine(void *arg) {
  pruning_service_t *ps = (pruning_service_t *)arg;
  tangle_t tangle;
  uint64_t start_timestamp, end_timestamp;
  uint64_t start_index;
  spent_addresses_provider_t sap;

  {
    connection_config_t db_conf = {.db_path = ps->conf->tangle_db_path};

    if (iota_tangle_init(&tangle, &db_conf) != RC_OK) {
      log_critical(logger_id, "Failed in initializing db\n");
      return NULL;
    }
  }

  {
    connection_config_t db_conf = {.db_path = ps->spent_addresses_service->conf->spent_addresses_db_path};

    if (ps->spent_addresses_service->conf->spent_addresses_files == NULL) {
      return RC_OK;
    }

    if (iota_spent_addresses_provider_init(&sap, &db_conf) != RC_OK) {
      log_error(logger_id, "Initializing spent addresses database connection failed\n");
      goto cleanup;
    }
  }

  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, milestone_pack);
  iota_tangle_milestone_load_first(&tangle, &milestone_pack);

  lock_handle_lock(&ps->lock_handle);
  if (milestone_pack.num_loaded > 0) {
    ps->last_pruned_snapshot_index = MIN(MIN(0UL, milestone.index - 1), ps->last_pruned_snapshot_index);
  }

  while (ps->running) {
    start_index = ps->last_pruned_snapshot_index;
    start_timestamp = current_timestamp_ms();
    while (ps->last_pruned_snapshot_index < ps->last_snapshot_index_to_prune) {
      if (prune_transactions(ps, &tangle, &sap) != RC_OK) {
        goto cleanup;
      }
      cond_handle_timedwait(&ps->cond_pruning_service, &ps->lock_handle, PRUNING_RESCAN_INTERVAL_SHORT_MS);
    }
    lock_handle_lock(&ps->lock_handle);
    end_timestamp = current_timestamp_ms();
    log_info(logger_id, "Pruning from % " PRIu64 " to % " PRIu64 " took % " PRIu64 " milliseconds\n", start_index,
             ps->last_pruned_snapshot_index, end_timestamp - start_timestamp);
    cond_handle_timedwait(&ps->cond_pruning_service, &ps->lock_handle, PRUNING_RESCAN_INTERVAL_MS);
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

static retcode_t collect_transactions_to_prune(pruning_service_t *const ps, tangle_t const *const tangle,
                                               spent_addresses_provider_t *const sap,
                                               flex_trit_t const *const entry_point_hash,
                                               hash243_set_t *const transactions_to_prune) {
  retcode_t err;
  DECLARE_PACK_SINGLE_TX(tx, txp, tx_pack);
  collect_transactions_for_pruning_do_func_params_t params = {.tangle = tangle,
                                                              .transactions_to_prune = transactions_to_prune};
  bool entry_point_has_no_solid_entry_points_in_past_cone = true;
  hash243_set_entry_t *iter = NULL, *tmp = NULL;
  bool spent;

  params.current_snapshot_index = ps->last_pruned_snapshot_index + 1;

  ERR_BIND_GOTO(tangle_traversal_dfs_to_past(params.tangle, collect_transactions_for_pruning_do_func, entry_point_hash,
                                             ps->conf->genesis_hash, transactions_to_prune, &params),
                err, cleanup);

  iter = NULL;
  tmp = NULL;

  HASH_ITER(hh, *params.transactions_to_prune, iter, tmp) {
    if (iota_snapshot_has_solid_entry_point(ps->new_snapshot, iter->hash)) {
      entry_point_has_no_solid_entry_points_in_past_cone = false;
      break;
    }
  }

  if (entry_point_has_no_solid_entry_points_in_past_cone) {
    HASH_ITER(hh, *params.transactions_to_prune, iter, tmp) {
      hash_pack_reset(&tx_pack);
      ERR_BIND_GOTO(
          iota_tangle_transaction_load_partial(tangle, iter->hash, &tx_pack, PARTIAL_TX_MODEL_ESSENCE_METADATA), err,
          cleanup);
      ERR_BIND_GOTO(iota_spent_addresses_service_was_tx_spent_from(sap, tangle, &tx, iter->hash, &spent), err, cleanup);
      ERR_BIND_GOTO(tips_cache_remove(ps->tips_cache, iter->hash), err, cleanup);
    }
  }

  if (!entry_point_has_no_solid_entry_points_in_past_cone && err == RC_OK) {
    hash243_set_free(params.transactions_to_prune);
  }

cleanup:
  if (err != RC_OK) {
    params.current_snapshot_index = ps->last_pruned_snapshot_index;
    hash243_set_free(params.transactions_to_prune);
  }
  return RC_OK;
}

static retcode_t prune_transactions(pruning_service_t *const ps, tangle_t const *const tangle,
                                    spent_addresses_provider_t *const sap) {
  retcode_t err;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, milestone_pack);
  hash243_set_t transactions_to_prune = NULL;
  size_t num_pruned_transactions;

  ERR_BIND_GOTO(iota_tangle_milestone_load_by_index(tangle, ps->last_pruned_snapshot_index + 1, &milestone_pack), err,
                cleanup);

  ERR_BIND_GOTO(collect_transactions_to_prune(ps, tangle, sap, milestone.hash, &transactions_to_prune), err, cleanup);
  hash243_set_remove(&transactions_to_prune, milestone.hash);
  ERR_BIND_GOTO(iota_tangle_transaction_delete_transactions(tangle, transactions_to_prune), err, cleanup);
  num_pruned_transactions = hash243_set_size(transactions_to_prune);
  hash243_set_free(&transactions_to_prune);
  // It's important to delete the milestone only after all it's past cone has been deleted to avoid dangle transactions
  hash243_set_add(&transactions_to_prune, milestone.hash);
  ERR_BIND_GOTO(iota_tangle_milestone_delete(tangle, milestone.hash), err, cleanup);
  log_info(logger_id,
           "Snapshot index % " PRIu64 " was pruned successfully, % " PRIu64 " transactions were removed from db\n",
           ps->last_pruned_snapshot_index, num_pruned_transactions + 1);
  ps->last_pruned_snapshot_index++;

  hash243_set_free(&transactions_to_prune);

cleanup:

  if (err != RC_OK) {
    lock_handle_unlock(&ps->lock_handle);
    log_warning(logger_id, "Local snapshots pruning has failed with error code: %d\n", err);
  }

  return RC_OK;
}

retcode_t iota_local_snapshots_pruning_service_init(pruning_service_t *const ps,
                                                    snapshots_provider_t *const snapshot_provider,
                                                    spent_addresses_service_t *const spent_addresses_service,
                                                    tips_cache_t *const tips_cache,
                                                    iota_consensus_conf_t const *const conf) {
  logger_id = logger_helper_enable(PRUNING_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);
  memset(ps, 0, sizeof(pruning_service_t));
  ps->conf = conf;
  ps->last_pruned_snapshot_index = snapshot_provider->inital_snapshot.metadata.index;
  ps->last_snapshot_index_to_prune = ps->last_pruned_snapshot_index;
  ps->spent_addresses_service = spent_addresses_service;
  ps->tips_cache = tips_cache;
  lock_handle_init(&ps->lock_handle);
  return RC_OK;
}

retcode_t iota_local_snapshots_pruning_service_start(pruning_service_t *const ps) {
  if (ps == NULL) {
    return RC_NULL_PARAM;
  }

  ps->running = true;

  log_info(logger_id, "Spawning pruning service thread\n");
  if (thread_handle_create(&ps->pruning_service_thread, (thread_routine_t)pruning_service_routine, ps) != 0) {
    log_critical(logger_id, "Spawning pruning service thread failed\n");
    return RC_THREAD_CREATE;
  }

  return RC_OK;
}

retcode_t iota_local_snapshots_pruning_service_stop(pruning_service_t *const ps) {
  retcode_t ret = RC_OK;

  if (ps == NULL) {
    return RC_NULL_PARAM;
  } else if (ps->running == false) {
    return RC_OK;
  }

  ps->running = false;
  cond_handle_signal(&ps->cond_pruning_service);
  log_info(logger_id, "Shutting down pruning service thread\n");
  if (thread_handle_join(ps->pruning_service_thread, NULL) != 0) {
    log_error(logger_id, "Shutting down pruning service thread failed\n");
    ret = RC_THREAD_JOIN;
  }

  return ret;
}

retcode_t iota_local_snapshots_pruning_service_destroy(pruning_service_t *const ps) {
  retcode_t ret = RC_OK;

  if (ps == NULL) {
    return RC_NULL_PARAM;
  } else if (ps->running) {
    return RC_STILL_RUNNING;
  }

  cond_handle_destroy(&ps->cond_pruning_service);
  memset(ps, 0, sizeof(pruning_service_t));
  logger_helper_release(logger_id);
  lock_handle_destroy(&ps->lock_handle);

  return ret;
}

void iota_local_snapshots_pruning_service_update_current_snapshot(pruning_service_t *const ps,
                                                                  snapshot_t *const snapshot) {
  lock_handle_lock(&ps->lock_handle);
  ps->last_snapshot_index_to_prune = snapshot->metadata.index;
  lock_handle_unlock(&ps->lock_handle);

  ps->new_snapshot = snapshot;
  cond_handle_signal(&ps->cond_pruning_service);
}
