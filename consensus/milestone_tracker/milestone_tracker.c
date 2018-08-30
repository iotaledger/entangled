/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "consensus/milestone_tracker/milestone_tracker.h"
#include "utils/logger_helper.h"

#define MILESTONE_TRACKER_LOGGER_ID "consensus_milestone_tracker"

static void* latest_milestone_tracker(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;
  return NULL;
}

static void* solid_milestone_tracker(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;
  return NULL;
}

retcode_t iota_milestone_tracker_init(milestone_tracker_t* const mt,
                                      tangle_t* const tangle) {
  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  } else if (tangle == NULL) {
    return RC_CONSENSUS_MT_NULL_TANGLE;
  }

  logger_helper_init(MILESTONE_TRACKER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(mt, 0, sizeof(milestone_tracker_t));
  mt->running = false;
  mt->tangle = tangle;

  return RC_OK;
}

retcode_t iota_milestone_tracker_start(milestone_tracker_t* const mt) {
  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  }

  mt->running = true;

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Spawning latest milestone tracker thread\n");
  if (thread_handle_create(&mt->latest_milestone_tracker,
                           (thread_routine_t)latest_milestone_tracker,
                           mt) != 0) {
    log_critical(MILESTONE_TRACKER_LOGGER_ID,
                 "Spawning latest milestone tracker thread failed\n");
    return RC_CONSENSUS_MT_FAILED_THREAD_SPAWN;
  }

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Spawning solid milestone tracker thread\n");
  if (thread_handle_create(&mt->solid_milestone_tracker,
                           (thread_routine_t)solid_milestone_tracker,
                           mt) != 0) {
    log_critical(MILESTONE_TRACKER_LOGGER_ID,
                 "Spawning solid milestone tracker thread failed\n");
    return RC_CONSENSUS_MT_FAILED_THREAD_SPAWN;
  }

  return RC_OK;
}

retcode_t iota_milestone_tracker_stop(milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;

  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  }

  mt->running = false;

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Shutting down latest milestone tracker thread\n");
  if (thread_handle_join(mt->latest_milestone_tracker, NULL) != 0) {
    log_error(MILESTONE_TRACKER_LOGGER_ID,
              "Shutting down latest milestone tracker thread failed\n");
    ret = RC_CONSENSUS_MT_FAILED_THREAD_JOIN;
  }

  log_info(MILESTONE_TRACKER_LOGGER_ID,
           "Shutting down solid milestone tracker thread\n");
  if (thread_handle_join(mt->solid_milestone_tracker, NULL) != 0) {
    log_error(MILESTONE_TRACKER_LOGGER_ID,
              "Shutting down solid milestone tracker thread failed\n");
    ret = RC_CONSENSUS_MT_FAILED_THREAD_JOIN;
  }

  return ret;
}

retcode_t iota_milestone_tracker_destroy(milestone_tracker_t* const mt) {
  retcode_t ret = RC_OK;

  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  } else if (mt->running) {
    return RC_CONSENSUS_MT_STILL_RUNNING;
  }

  memset(mt, 0, sizeof(milestone_tracker_t));
  logger_helper_destroy(MILESTONE_TRACKER_LOGGER_ID);

  return ret;
}
