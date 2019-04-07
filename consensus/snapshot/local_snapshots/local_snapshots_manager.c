/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/local_snapshots/local_snapshots_manager.h"
#include "common/storage/connection.h"
#include "consensus/snapshot/local_snapshots/conf.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define LOCAL_SNAPSHOTS_RESCAN_INTERVAL_MS 10000

#define LOCAL_SNAPSHOTS_MANAGER_LOGGER_ID "local_snapshots_manager"

static logger_id_t logger_id;

static void *local_snapshots_manager_routine(void *arg) {
  local_snapshots_manager_t *lsm = (local_snapshots_manager_t *)arg;
  lock_handle_t lock_cond;
  retcode_t err;
  size_t transactions_count;
  size_t exponential_delay_factor = 1;

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (lsm->running) {
    if (iota_local_snapshots_manager_should_take_snapshot(lsm)) {
      err = iota_local_snapshots_manager_take_snapshot(lsm);
      if (err == RC_OK) {
        exponential_delay_factor = 1;
      } else if (err == RC_LEDGER_VALIDATOR_TRANSACTION_NOT_SOLID ||
                 err == RC_SNAPSHOT_LOCAL_SNAPSHOTS_NOT_ENOUGH_DEPTH) {
        exponential_delay_factor *= 2;
        log_warning(logger_id, "Local snapshot is delayed in %d ms, error code: %d\n",
                    exponential_delay_factor * LOCAL_SNAPSHOTS_RESCAN_INTERVAL_MS, err);
      } else {
        goto cleanup;
      }
    }
    cond_handle_timedwait(&lsm->cond_local_snapshots, &lock_cond,
                          exponential_delay_factor * LOCAL_SNAPSHOTS_RESCAN_INTERVAL_MS);
  }

cleanup:

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  return NULL;
}

bool iota_local_snapshots_manager_should_take_snapshot(local_snapshots_manager_t const *const lsm) {
  size_t new_transactions_count;
  if (iota_tangle_transaction_count(&lsm->tangle, &new_transactions_count) != RC_OK) {
    log_critical(logger_id, "Failed in querying db size\n");
    return false;
  }
  if (((new_transactions_count - lsm->last_snapshot_transactions_count) >=
       lsm->conf->local_snapshots.transactions_growth_threshold) &&
      (lsm->mt->latest_milestone_index - lsm->local_snapshots_provider.start_snapshot_index) >
          lsm->conf->local_snapshots.local_snapshot_min_depth) {
    return true;
  }
  return false;
}

retcode_t iota_local_snapshots_manager_take_snapshot(local_snapshots_manager_t *const lsm) {
  retcode_t err;

  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  ERR_BIND_RETURN(iota_local_snapshots_manager_determine_new_entry_point(lsm, &pack), err);

  // TODO - implement + uncomment
  // ERR_BIND_RETURN(iota_tangle_transaction_count(&lsm->tangle, &lsm->last_snapshot_transactions_count),err);
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}

retcode_t iota_local_snapshots_manager_determine_new_entry_point(local_snapshots_manager_t *const lsm,
                                                                 iota_stor_pack_t *const entry_point) {
  retcode_t err;
  uint64_t index = lsm->mt->latest_milestone_index - lsm->conf->local_snapshots.local_snapshot_min_depth - 1;
  if (index == 0) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_NOT_ENOUGH_DEPTH;
  }
  ERR_BIND_RETURN(iota_tangle_milestone_load_next(&lsm->tangle, index, entry_point), err);
  return RC_OK;
}

retcode_t iota_local_snapshots_manager_init(local_snapshots_manager_t *lsm, iota_consensus_conf_t const *const conf,
                                            milestone_tracker_t const *const mt) {
  retcode_t err;
  if (lsm == NULL) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_NULL_SELF;
  }

  if (mt == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(LOCAL_SNAPSHOTS_MANAGER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(lsm, 0, sizeof(local_snapshots_manager_t));
  lsm->running = false;
  lsm->conf = conf;
  lsm->mt = mt;

  cond_handle_init(&lsm->cond_local_snapshots);

  connection_config_t db_conf = {.db_path = lsm->conf->db_path};

  ERR_BIND_RETURN(iota_tangle_init(&lsm->tangle, &db_conf), err);

  return RC_OK;
}

retcode_t iota_local_snapshots_manager_start(local_snapshots_manager_t *const lsm) {
  retcode_t err;
  if (lsm == NULL) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_NULL_SELF;
  }

  lsm->running = true;

  ERR_BIND_RETURN(iota_tangle_transaction_count(&lsm->tangle, &lsm->last_snapshot_transactions_count), err);

  cond_handle_signal(&lsm->cond_local_snapshots);
  log_info(logger_id, "Spawning local snapshots manager thread\n");
  if (thread_handle_create(&lsm->local_snapshots_thread, (thread_routine_t)local_snapshots_manager_routine, lsm) != 0) {
    log_critical(logger_id, "Spawning local snapshots manager thread failed\n");
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t iota_local_snapshots_manager_stop(local_snapshots_manager_t *const lsm) {
  retcode_t ret = RC_OK;

  if (lsm == NULL) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_NULL_SELF;
  } else if (lsm->running == false) {
    return RC_OK;
  }

  lsm->running = false;

  log_info(logger_id, "Shutting down local snapshots manager thread\n");
  if (thread_handle_join(lsm->local_snapshots_thread, NULL) != 0) {
    log_error(logger_id, "Shutting down local snapshots manager thread failed\n");
    ret = RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_FAILED_THREAD_JOIN;
  }

  return ret;
}

retcode_t iota_local_snapshots_manager_destroy(local_snapshots_manager_t *const lsm) {
  retcode_t ret = RC_OK;

  if (lsm == NULL) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_NULL_SELF;
  } else if (lsm->running) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_STILL_RUNNING;
  }

  if (iota_tangle_destroy(&lsm->tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  cond_handle_destroy(&lsm->cond_local_snapshots);
  memset(lsm, 0, sizeof(local_snapshots_manager_t));
  logger_helper_release(logger_id);

  return ret;
}