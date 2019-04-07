/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_SNAPSHOTS_PROVIDER_H__
#define __CONSENSUS_SNAPSHOT_SNAPSHOTS_PROVIDER_H__

#include <stdbool.h>
#include <stdint.h>

#include "consensus/conf.h"
#include "consensus/snapshot/snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct local_snapshots_provider_s {
  // This snapshot is an entry point and it's always updated with the last snapshot after its being persisted
  snapshot_t inital_snapshot;
  snapshot_t latest_snapshot;
} snapshots_provider_t;

/**
 * Initializes a snapshots provider
 *
 * @param snapshots_provider The snapshots provider
 * @param conf The snapshot configuration
 *
 * @return a status code
 */
retcode_t iota_snapshots_provider_init(snapshots_provider_t *snapshots_provider,
                                       iota_consensus_conf_t const *const conf);

/**
 * Destroys a snapshots provider
 *
 * @param snapshots_provider The local snapshots provider
 *
 * @return a status code
 */
retcode_t iota_snapshots_provider_destroy(snapshots_provider_t *snapshots_provider);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_SNAPSHOTS_PROVIDER_H__
