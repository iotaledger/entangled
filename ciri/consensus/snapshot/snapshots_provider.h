/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_SNAPSHOTS_PROVIDER_H__
#define __CONSENSUS_SNAPSHOT_SNAPSHOTS_PROVIDER_H__

#include <stdbool.h>
#include <stdint.h>

#include "ciri/consensus/conf.h"
#include "ciri/consensus/snapshot/snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct snapshots_provider_s {
  snapshot_t initial_snapshot;
  // This snapshot is an entry point and it's always updated with the last snapshot after being persisted
  snapshot_t latest_snapshot;
} snapshots_provider_t;

/**
 * Initializes a snapshots provider
 *
 * @param snapshots_provider The snapshots provider
 * @param conf The consensus configuration
 *
 * @return a status code
 */
retcode_t iota_snapshots_provider_init(snapshots_provider_t *snapshots_provider, iota_consensus_conf_t *const conf);

/**
 * Destroys a snapshots provider
 *
 * @param snapshots_provider The snapshots provider
 *
 * @return a status code
 */
retcode_t iota_snapshots_provider_destroy(snapshots_provider_t *const snapshots_provider);

/**
 * Writes snapshot to file
 *
 * @param snapshots_provider The snapshots provider
 * @param snapshot The snapshot to write
 *
 * @return a status code
 */
retcode_t iota_snapshots_provider_write_snapshot_to_file(snapshot_t *const snapshot, char const *const file_path_base);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_SNAPSHOTS_PROVIDER_H__
