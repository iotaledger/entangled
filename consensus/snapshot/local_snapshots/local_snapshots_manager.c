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

  tangle_t tangle;
  connection_config_t db_conf = {.db_path = lsm->conf->db_path};
  if (err = iota_tangle_init(&tangle, &db_conf)) {
    log_critical(logger_id, "Failed in initializing db\n");
    return NULL;
  }

  while (lsm->running) {
    if (iota_local_snapshots_manager_should_take_snapshot(lsm, &tangle)) {
      err = iota_snapshots_service_take_snapshot(lsm->snapshots_service, lsm->mt);
      if (err == RC_OK) {
        exponential_delay_factor = 1;
        if (iota_tangle_transaction_count(&tangle, &lsm->last_snapshot_transactions_count) != RC_OK) {
          log_critical(logger_id, "Failed in querying db size\n");
          goto cleanup;
        }
      } else if (err == RC_SNAPSHOT_SERVICE_MILESTONE_NOT_SOLID || err == RC_SNAPSHOT_SERVICE_NOT_ENOUGH_DEPTH) {
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
  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  return NULL;
}

bool iota_local_snapshots_manager_should_take_snapshot(local_snapshots_manager_t const *const lsm,
                                                       tangle_t const *const tangle) {
  size_t new_transactions_count;
  if (iota_tangle_transaction_count(tangle, &new_transactions_count) != RC_OK) {
    log_critical(logger_id, "Failed in querying db size\n");
    return false;
  }

  if (((new_transactions_count - lsm->last_snapshot_transactions_count) >=
       lsm->conf->local_snapshots.transactions_growth_threshold) &&
      (lsm->snapshots_provider->latest_snapshot.index - lsm->snapshots_provider->inital_snapshot.index) >
          lsm->conf->local_snapshots.min_depth) {
    return true;
  }
  return false;
}

retcode_t iota_local_snapshots_manager_init(local_snapshots_manager_t *lsm,
                                            snapshots_provider_t *const snapshots_provider,
                                            iota_consensus_conf_t *const conf, milestone_tracker_t const *const mt) {
  retcode_t err;
  if (lsm == NULL) {
    return RC_NULL_PARAM;
  }

  if (mt == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(LOCAL_SNAPSHOTS_MANAGER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(lsm, 0, sizeof(local_snapshots_manager_t));
  lsm->running = false;
  lsm->conf = conf;
  lsm->mt = mt;
  lsm->snapshots_provider = snapshots_provider;

  cond_handle_init(&lsm->cond_local_snapshots);

  return RC_OK;
}

retcode_t iota_local_snapshots_manager_start(local_snapshots_manager_t *const lsm) {
  retcode_t err;
  if (lsm == NULL) {
    return RC_NULL_PARAM;
  }

  lsm->running = true;

  cond_handle_signal(&lsm->cond_local_snapshots);
  log_info(logger_id, "Spawning local snapshots manager thread\n");
  if (thread_handle_create(&lsm->local_snapshots_thread, (thread_routine_t)local_snapshots_manager_routine, lsm) != 0) {
    log_critical(logger_id, "Spawning local snapshots manager thread failed\n");
    return RC_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t iota_local_snapshots_manager_stop(local_snapshots_manager_t *const lsm) {
  retcode_t ret = RC_OK;

  if (lsm == NULL) {
    return RC_NULL_PARAM;
  } else if (lsm->running == false) {
    return RC_OK;
  }

  lsm->running = false;

  log_info(logger_id, "Shutting down local snapshots manager thread\n");
  if (thread_handle_join(lsm->local_snapshots_thread, NULL) != 0) {
    log_error(logger_id, "Shutting down local snapshots manager thread failed\n");
    ret = RC_FAILED_THREAD_JOIN;
  }

  return ret;
}

retcode_t iota_local_snapshots_manager_destroy(local_snapshots_manager_t *const lsm) {
  retcode_t ret = RC_OK;

  if (lsm == NULL) {
    return RC_NULL_PARAM;
  } else if (lsm->running) {
    return RC_STILL_RUNNING;
  }

  cond_handle_destroy(&lsm->cond_local_snapshots);
  memset(lsm, 0, sizeof(local_snapshots_manager_t));
  logger_helper_release(logger_id);

  return ret;
}