/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_CONF_H__
#define __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_CONF_H__

#include <stdbool.h>

#include "ciri/utils/files.h"
#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iota_consensus_local_snapshots_conf_s {
  bool local_snapshots_is_enabled;
  bool pruning_is_enabled;
  size_t transactions_growth_threshold;
  size_t min_depth;
  char base_dir[FILE_PATH_SIZE];
} iota_consensus_local_snapshots_conf_t;

/**
 * Initializes local snapshots configuration with default values
 *
 * @param conf LS configuration variables
 *
 * @return a status code
 */
retcode_t iota_consensus_local_snapshots_conf_init(iota_consensus_local_snapshots_conf_t* const conf);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_CONF_H__
