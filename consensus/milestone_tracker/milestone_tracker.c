/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/milestone_tracker/milestone_tracker.h"
#include "utils/logger_helper.h"

#define MILESTONE_TRACKER_LOGGER_ID "consensus_milestone_tracker"

retcode_t iota_milestone_tracker_init(milestone_tracker_t* const mt) {
  logger_helper_init(MILESTONE_TRACKER_LOGGER_ID, LOGGER_DEBUG, true);
  return RC_OK;
}

retcode_t iota_milestone_tracker_start(milestone_tracker_t* const mt) {
  return RC_OK;
}

retcode_t iota_milestone_tracker_stop(milestone_tracker_t* const mt) {
  return RC_OK;
}

retcode_t iota_milestone_tracker_destroy(milestone_tracker_t* const mt) {
  logger_helper_destroy(MILESTONE_TRACKER_LOGGER_ID);
  return RC_OK;
}
