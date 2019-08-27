/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/consensus/snapshot/local_snapshots/conf.h"
#include "utils/logger_helper.h"

#define LOCAL_SNAPSHOT_TRANSACTIONS_GROWTH_THRESHOLD 1000
#define LOCAL_SNAPSHOT_MIN_DEPTH 100
#define LOCAL_SNAPSHOTS_BASE_DIR "local_snapshot"

retcode_t iota_consensus_local_snapshots_conf_init(iota_consensus_local_snapshots_conf_t* const conf) {
  retcode_t ret = RC_OK;

  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  conf->local_snapshots_is_enabled = false;
  conf->pruning_is_enabled = false;
  conf->transactions_growth_threshold = LOCAL_SNAPSHOT_TRANSACTIONS_GROWTH_THRESHOLD;
  strcpy(conf->base_dir, LOCAL_SNAPSHOTS_BASE_DIR);
  conf->min_depth = LOCAL_SNAPSHOT_MIN_DEPTH;

  return ret;
}
