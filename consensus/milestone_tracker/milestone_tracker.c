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

  return RC_OK;
}

retcode_t iota_milestone_tracker_stop(milestone_tracker_t* const mt) {
  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  }

  return RC_OK;
}

retcode_t iota_milestone_tracker_destroy(milestone_tracker_t* const mt) {
  if (mt == NULL) {
    return RC_CONSENSUS_MT_NULL_SELF;
  }

  memset(mt, 0, sizeof(milestone_tracker_t));
  logger_helper_destroy(MILESTONE_TRACKER_LOGGER_ID);
  return RC_OK;
}
