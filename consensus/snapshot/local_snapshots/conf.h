/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_CONF_H__
#define __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_CONF_H__

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

// This structure contains all configuration variables needed to operate the
// IOTA consensus
typedef struct iota_consensus_local_snapshots_conf_s {
  bool local_snapshots_is_enabled;
  bool prunning_is_enabled;
  size_t transactions_growth_threshold;
  size_t min_depth;
  char local_snapshots_path_base[128];
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
