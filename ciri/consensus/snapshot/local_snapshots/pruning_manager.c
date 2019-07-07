/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/snapshot/local_snapshots/pruning_manager.h"
#include "common/errors.h"
#include "utils/logger_helper.h"

#define PRUNING_MANAGER_LOGGER_ID "pruning_manager"

static logger_id_t logger_id;

static void *pruning_manager_routine(void *arg) {
  pruning_manager_t *pm = (pruning_manager_t *)arg;
  retcode_t err;
  return NULL;
}

retcode_t iota_local_snapshots_pruning_manager_init(pruning_manager_t *pm) {
  logger_id = logger_helper_enable(PRUNING_MANAGER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(pm, 0, sizeof(pruning_manager_t));
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

  return ret;
}