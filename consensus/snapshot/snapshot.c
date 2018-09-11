/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshot.h"
#include "utils/logger_helper.h"

#define SNAPSHOT_LOGGER_ID "consensus_snapshot"

retcode_t iota_snapshot_init(snapshot_t *const snapshot) {
  logger_helper_init(SNAPSHOT_LOGGER_ID, LOGGER_DEBUG, true);
  return RC_OK;
}

retcode_t iota_snapshot_destroy(snapshot_t *const snapshot) {
  logger_helper_destroy(SNAPSHOT_LOGGER_ID);
  return RC_OK;
}
