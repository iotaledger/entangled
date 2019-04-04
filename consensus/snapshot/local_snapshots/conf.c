/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/local_snapshots/conf.h"

#include "utils/logger_helper.h"

#define CONSENSUS_LOCAL_SNAPSHOTS_CONF_LOGGER_ID "consensus_local_snapshots_conf"

static logger_id_t logger_id;

retcode_t iota_consensus_local_snapshots_conf_init(iota_consensus_local_snapshots_conf_t* const conf) {
  retcode_t ret = RC_OK;

  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  conf->local_snapshots_is_enabled = false;
  conf->prunning_is_enabled = false;

  logger_id = logger_helper_enable(CONSENSUS_LOCAL_SNAPSHOTS_CONF_LOGGER_ID, LOGGER_DEBUG, true);
  logger_helper_release(logger_id);

  return ret;
}
