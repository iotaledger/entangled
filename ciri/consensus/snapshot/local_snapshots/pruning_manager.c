/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/snapshot/local_snapshots/pruning_manager.h"
#include "ciri/consensus/snapshot/snapshots_provider.h"
#include "ciri/consensus/tangle/traversal.h"
#include "common/errors.h"
#include "common/model/milestone.h"
#include "utils/logger_helper.h"

#define PRUNING_MANAGER_LOGGER_ID "pruning_manager"

static logger_id_t logger_id;

typedef struct collect_transactions_for_pruning_do_func_params_s {
  uint64_t min_snapshot_index;
  tangle_t const *tangle;
  hash243_set_t transactions_to_prune;
} collect_transactions_for_pruning_do_func_params_t;

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

  if (current_snapshot_index >= params->min_snapshot_index) {
    *should_branch = true;
    if ((ret = hash243_set_add(&params->transactions_to_prune, hash)) != RC_OK) {
      return ret;
    }
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
  retcode_t err;
  tangle_t tangle;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  {
    connection_config_t db_conf = {.db_path = pm->conf->tangle_db_path};

    if ((err = iota_tangle_init(&tangle, &db_conf))) {
      log_critical(logger_id, "Failed in initializing db\n");
      return NULL;
    }
  }

  lock_handle_lock(&pm->rw_lock);
  while (pm->running) {
    while (pm->current_snapshot_index_to_prune < pm->last_snapshot_index_to_prune) {
      collect_transactions_for_pruning_do_func_params_t params;
      params.min_snapshot_index = ++pm->current_snapshot_index_to_prune;
      params.tangle = &tangle;
      ERR_BIND_GOTO(iota_tangle_milestone_load_by_index(&tangle, pm->current_snapshot_index_to_prune, &pack), err,
                    cleanup);
      ERR_BIND_GOTO(tangle_traversal_dfs_to_past(&tangle, collect_transactions_for_pruning_do_func, milestone.hash,
                                                 pm->conf->genesis_hash, NULL, &params),
                    err, cleanup);
      // TODO - remove milestone from tangle
      // TODO - create job to delete future cone of non confirmed txs of each tx in set
      hash243_set_free(&params.transactions_to_prune);
    }
    cond_handle_wait(&pm->cond_pruning_manager, &pm->rw_lock);
  }

cleanup:

  lock_handle_unlock(&pm->rw_lock);

  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  if (err != RC_OK) {
    log_warning(logger_id, "Local snapshots pruning has failed with error code: %d\n", err);
  }

  return NULL;
}

retcode_t iota_local_snapshots_pruning_manager_init(pruning_manager_t *pm,
                                                    snapshots_provider_t *const snapshot_provider,
                                                    iota_consensus_conf_t *const conf) {
  logger_id = logger_helper_enable(PRUNING_MANAGER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(pm, 0, sizeof(pruning_manager_t));
  pm->conf = conf;
  pm->current_snapshot_index_to_prune = snapshot_provider->inital_snapshot.metadata.index;
  pm->last_snapshot_index_to_prune = pm->current_snapshot_index_to_prune;
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

void iota_local_snapshots_pruning_manager_update_last_snapshot_index(pruning_manager_t *const pm,
                                                                     uint64_t last_snapshot_index) {
  rw_lock_handle_wrlock(&pm->rw_lock);
  pm->last_snapshot_index_to_prune = last_snapshot_index;
  rw_lock_handle_unlock(&pm->rw_lock);
  cond_handle_signal(&pm->cond_pruning_manager);
}
