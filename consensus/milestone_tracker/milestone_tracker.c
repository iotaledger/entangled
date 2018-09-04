/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <string.h>

#include "ciri/conf/conf_values.h"
#include "common/storage/pack.h"
#include "common/trinary/trit_array.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define MILESTONE_TRACKER_LOGGER_ID "consensus_milestone_tracker"


static void* latest_milestone_tracker(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;

  while (mt->running) {
    log_debug(MILESTONE_TRACKER_LOGGER_ID, "Scanning for latest milestone\n");
    uint64_t scan_time = current_timestamp_ms();
    uint64_t previous_latest_milestone_index = mt->latest_milestone_index;
    iota_stor_pack_t hash_pack;
    hash_pack_init(&hash_pack, 20);
    for (size_t i = 0; i < hash_pack.num_loaded; ++i) {
    }
    if (previous_latest_milestone_index != mt->latest_milestone_index) {
      // TODO messageQ publish lmi
      log_info(MILESTONE_TRACKER_LOGGER_ID,
               "Latest milestone has changed from #%" PRIu64 " to #%" PRIu64
               "\n",
               previous_latest_milestone_index, mt->latest_milestone_index);
    }
    sleep_ms(
        MAX(1, LMT_RESCAN_INTERVAL - (current_timestamp_ms() - scan_time)));
  }
  return NULL;
}

static void* solid_milestone_tracker(void* arg) {
  milestone_tracker_t* mt = (milestone_tracker_t*)arg;
  while (mt->running) {
    log_debug(MILESTONE_TRACKER_LOGGER_ID,
              "Scanning for latest solid subtangle milestone\n");
    uint64_t scan_time = current_timestamp_ms();
    uint64_t previous_solid_subtangle_latest_milestone_index =
        mt->latest_solid_subtangle_milestone_index;

    if (mt->latest_solid_subtangle_milestone_index <
        mt->latest_milestone_index) {
      // TODO updateLatestSolidSubtangleMilestone();
    }
    if (previous_solid_subtangle_latest_milestone_index !=
        mt->latest_solid_subtangle_milestone_index) {
      // TODO messageQ publish lmsi/lmhs
      log_info(MILESTONE_TRACKER_LOGGER_ID,
               "Latest solid subtangle milestone has changed from #%" PRIu64
               " to #%" PRIu64 "\n",
               previous_solid_subtangle_latest_milestone_index,
               mt->latest_solid_subtangle_milestone_index);
    }
    sleep_ms(
        MAX(1, SMT_RESCAN_INTERVAL - (current_timestamp_ms() - scan_time)));
  }
  return NULL;
}

retcode_t iota_milestone_tracker_init(milestone_tracker_t* const mt,
                                      tangle_t* const tangle, bool testnet) {
  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  } else if (tangle == NULL) {
    return RC_CONSENSUS_MT_NULL_TANGLE;
  }

  logger_helper_init(MILESTONE_TRACKER_LOGGER_ID, LOGGER_DEBUG, true);
  memset(mt, 0, sizeof(milestone_tracker_t));
  mt->running = false;
  mt->testnet = testnet;
  mt->tangle = tangle;
  if ((mt->latest_milestone = trit_array_new(NUM_TRITS_HASH)) == NULL) {
    goto oom;
  }
  if ((mt->latest_solid_subtangle_milestone = trit_array_new(NUM_TRITS_HASH)) ==
      NULL) {
    goto oom;
  }
  if (mt->testnet) {
    if ((mt->coordinator = trit_array_new_from_trytes(
             (tryte_t*)TESTNET_COORDINATOR_ADDRESS)) == NULL) {
      goto oom;
    }
    mt->milestone_start_index = TESTNET_MILESTONE_START_INDEX;
  } else {
    if ((mt->coordinator = trit_array_new_from_trytes(
             (tryte_t*)MAINNET_COORDINATOR_ADDRESS)) == NULL) {
      goto oom;
    }
    mt->milestone_start_index = MAINNET_MILESTONE_START_INDEX;
  }
  mt->latest_milestone_index = mt->milestone_start_index;
  mt->latest_solid_subtangle_milestone_index = mt->milestone_start_index;

  return RC_OK;

oom:
  if (mt->latest_milestone) {
    free(mt->latest_milestone);
  }
  if (mt->latest_solid_subtangle_milestone) {
    free(mt->latest_solid_subtangle_milestone);
  }
  if (mt->coordinator) {
    free(mt->coordinator);
  }
  return RC_CONSENSUS_MT_OOM;
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

  trit_array_free(mt->latest_milestone);
  trit_array_free(mt->latest_solid_subtangle_milestone);
  trit_array_free(mt->coordinator);
  memset(mt, 0, sizeof(milestone_tracker_t));
  logger_helper_destroy(MILESTONE_TRACKER_LOGGER_ID);

  return ret;
}
