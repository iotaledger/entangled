/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/snapshot/local_snapshots/local_snapshots_manager.h"
#include <inttypes.h>
#include "ciri/consensus/snapshot/local_snapshots/conf.h"
#include "ciri/consensus/snapshot/snapshots_service.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"
#include "utils/time.h"

#define LOCAL_SNAPSHOTS_RESCAN_INTERVAL_MS 10000

#define LOCAL_SNAPSHOTS_MANAGER_LOGGER_ID "local_snapshots_manager"

static logger_id_t logger_id;

static void *local_snapshots_manager_routine(void *arg) {
  local_snapshots_manager_t *lsm = (local_snapshots_manager_t *)arg;
  retcode_t err;
  size_t exponential_delay_factor = 1;
  uint64_t start_timestamp, end_timestamp;
  bool skip_check = false;
  tangle_t tangle;
  lock_handle_t lock_cond;
  uint64_t prev_initial_index;
  uint64_t initial_delta_size;

  {
    storage_connection_config_t db_conf = {.db_path = lsm->conf->tangle_db_path};

    if ((err = iota_tangle_init(&tangle, &db_conf))) {
      log_critical(logger_id, "Failed in initializing db\n");
      return NULL;
    }
  }

  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);

  while (lsm->running) {
    if (skip_check || iota_local_snapshots_manager_should_take_snapshot(lsm, &tangle)) {
      start_timestamp = current_timestamp_ms();
      prev_initial_index = lsm->snapshots_service->snapshots_provider->initial_snapshot.metadata.index;
      initial_delta_size = state_delta_size(lsm->snapshots_service->snapshots_provider->initial_snapshot.state);
      err = iota_snapshots_service_take_snapshot(lsm->snapshots_service, &lsm->ps, &tangle);
      if (err == RC_OK) {
        exponential_delay_factor = 1;
        end_timestamp = current_timestamp_ms();
        if (iota_tangle_transaction_count(&tangle, &lsm->last_snapshot_transactions_count) != RC_OK) {
          log_critical(logger_id, "Failed in querying db size\n");
          goto cleanup;
        }
        log_info(logger_id,
                 "Local snapshot from %" PRId64 " to %" PRId64 " took %" PRId64
                 " milliseconds\nState delta size before snapshot was: %" PRId64 " and now is: %" PRId64 " \n",
                 prev_initial_index, lsm->snapshots_service->snapshots_provider->initial_snapshot.metadata.index,
                 end_timestamp - start_timestamp, initial_delta_size,
                 state_delta_size(lsm->snapshots_service->snapshots_provider->initial_snapshot.state));
      } else {
        exponential_delay_factor *= 2;
        log_warning(logger_id, "Local snapshot is delayed in %d ms, error code: %d\n",
                    exponential_delay_factor * LOCAL_SNAPSHOTS_RESCAN_INTERVAL_MS, err);
        cond_handle_timedwait(&lsm->cond_local_snapshots, &lock_cond,
                              exponential_delay_factor * LOCAL_SNAPSHOTS_RESCAN_INTERVAL_MS);
      }
    }
    if (lsm->running && !iota_local_snapshots_manager_should_take_snapshot(lsm, &tangle)) {
      cond_handle_timedwait(&lsm->cond_local_snapshots, &lock_cond, LOCAL_SNAPSHOTS_RESCAN_INTERVAL_MS);
      skip_check = false;
    } else {
      skip_check = true;
    }
  }

cleanup:

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);
  if (iota_tangle_destroy(&tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  if (err != RC_OK) {
    log_warning(logger_id, "Local snapshot is failed with error code: %d\n", err);
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

  uint64_t latest_to_initial_gap = lsm->snapshots_service->snapshots_provider->latest_snapshot.metadata.index -
                                   lsm->snapshots_service->snapshots_provider->initial_snapshot.metadata.index;

  if ((latest_to_initial_gap > SNAPSHOT_SERVICE_MAX_NUM_MILESTONES_TO_CALC) ||
      (((new_transactions_count - lsm->last_snapshot_transactions_count) >=
        lsm->conf->local_snapshots.transactions_growth_threshold) &&
       (latest_to_initial_gap > 2 * lsm->conf->local_snapshots.min_depth))) {
    return true;
  }

  return false;
}

retcode_t iota_local_snapshots_manager_init(local_snapshots_manager_t *lsm,
                                            snapshots_service_t *const snapshots_service,
                                            iota_consensus_conf_t *const conf, milestone_tracker_t const *const mt,
                                            spent_addresses_service_t *const spent_addresses_service,
                                            tips_cache_t *const tips_cache) {
  retcode_t ret = RC_OK;
  if (lsm == NULL || mt == NULL || snapshots_service == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(LOCAL_SNAPSHOTS_MANAGER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(lsm, 0, sizeof(local_snapshots_manager_t));
  lsm->running = false;
  lsm->conf = conf;
  lsm->mt = mt;
  lsm->snapshots_service = snapshots_service;

  cond_handle_init(&lsm->cond_local_snapshots);

  if (lsm->conf->local_snapshots.pruning_is_enabled) {
    ERR_BIND_RETURN(iota_local_snapshots_pruning_service_init(&lsm->ps, lsm->snapshots_service->snapshots_provider,
                                                              spent_addresses_service, tips_cache, conf),
                    ret);
  }

  return ret;
}

retcode_t iota_local_snapshots_manager_start(local_snapshots_manager_t *const lsm) {
  retcode_t ret;
  if (lsm == NULL) {
    return RC_NULL_PARAM;
  }

  lsm->running = true;

  log_info(logger_id, "Spawning local snapshots manager thread\n");
  if (thread_handle_create(&lsm->local_snapshots_thread, (thread_routine_t)local_snapshots_manager_routine, lsm) != 0) {
    log_critical(logger_id, "Spawning local snapshots manager thread failed\n");
    return RC_THREAD_CREATE;
  }

  if (lsm->conf->local_snapshots.pruning_is_enabled) {
    ERR_BIND_RETURN(iota_local_snapshots_pruning_service_start(&lsm->ps), ret);
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

  if (lsm->conf->local_snapshots.pruning_is_enabled) {
    ret = iota_local_snapshots_pruning_service_stop(&lsm->ps);
  }

  lsm->running = false;
  cond_handle_signal(&lsm->cond_local_snapshots);
  log_info(logger_id, "Shutting down local snapshots manager thread\n");
  if (thread_handle_join(lsm->local_snapshots_thread, NULL) != 0) {
    log_error(logger_id, "Shutting down local snapshots manager thread failed\n");
    // We want to return the first error
    if (ret == RC_OK) {
      ret = RC_THREAD_JOIN;
    }
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

  if (lsm->conf->local_snapshots.pruning_is_enabled) {
    ret = iota_local_snapshots_pruning_service_destroy(&lsm->ps);
  }

  cond_handle_destroy(&lsm->cond_local_snapshots);
  memset(lsm, 0, sizeof(local_snapshots_manager_t));
  logger_helper_release(logger_id);

  return ret;
}
