/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/local_snapshots/local_snapshots_manager.h"
#include "consensus/snapshot/local_snapshots/conf.h"
#include "utils/logger_helper.h"

#define LOCAL_SNAPSHOTS_RESCAN_INTERVAL_MS 10000

#define LOCAL_SNAPSHOTS_MANAGER_LOGGER_ID "local_snapshots_manager"

static logger_id_t logger_id;

static void* local_snapshots_manager_routine(void* arg) {
  local_snapshots_manager_t* lsm = (local_snapshots_manager_t*)arg;
  lock_handle_t lock_cond;
  retcode_t err;
  lock_handle_init(&lock_cond);
  lock_handle_lock(&lock_cond);
  while (lsm->running) {
    if (local_snapshots_should_take_snapshot(lsm)) {
      ERR_BIND_GOTO(local_snapshots_take_snapshot(lsm), err, cleanup);
    }
    cond_handle_timedwait(&lsm->cond_local_snapshots, &lock_cond, LOCAL_SNAPSHOTS_RESCAN_INTERVAL_MS);
  }

cleanup:

  lock_handle_unlock(&lock_cond);
  lock_handle_destroy(&lock_cond);

  return NULL;
}

bool local_snapshots_should_take_snapshot(local_snapshots_manager_t const* const lsm) { return false; }

retcode_t local_snapshots_take_snapshot(local_snapshots_manager_t* const lsm) { return RC_CONSENSUS_NOT_IMPLEMENTED; }

retcode_t iota_local_snapshots_init(local_snapshots_manager_t* lsm, iota_consensus_conf_t const* const conf,
                                    milestone_tracker_t const* const mt) {
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

  return RC_OK;
}

retcode_t iota_local_snapshots_start(local_snapshots_manager_t* const lsm) {
  if (lsm == NULL) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_NULL_SELF;
  }

  lsm->running = true;

  cond_handle_signal(&lsm->cond_local_snapshots);
  log_info(logger_id, "Spawning local snapshots manager thread\n");
  if (thread_handle_create(&lsm->local_snapshots_thread, (thread_routine_t)local_snapshots_manager_routine, lsm) != 0) {
    log_critical(logger_id, "Spawning local snapshots manager thread failed\n");
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t iota_local_snapshots_stop(local_snapshots_manager_t* const lsm) {
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

retcode_t iota_local_snapshots_destroy(local_snapshots_manager_t* const lsm) {
  retcode_t ret = RC_OK;

  if (lsm == NULL) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_NULL_SELF;
  } else if (lsm->running) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MANAGER_STILL_RUNNING;
  }

  cond_handle_destroy(&lsm->cond_local_snapshots);
  memset(lsm, 0, sizeof(local_snapshots_manager_t));
  logger_helper_release(logger_id);

  return ret;
}