/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_SERVICE_H__
#define __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_SERVICE_H__

#include <stdbool.h>
#include <stdint.h>

#include "ciri/consensus/milestone/milestone_service.h"
#include "ciri/consensus/milestone/milestone_tracker.h"
#include "ciri/consensus/snapshot/local_snapshots/pruning_service.h"
#include "ciri/consensus/snapshot/snapshots_provider.h"
#include "ciri/consensus/tangle/tangle.h"
#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maximum age in milestones since creation of solid entry points.
 *
 * Since it is possible to artificially keep old solid entry points alive by periodically attaching new transactions
 * to them, we limit the life time of solid entry points and ignore them whenever they become too old. This is a
 * measure against a potential attack vector where somebody might try to blow up the meta data of local snapshots.
 */

#define SNAPSHOT_SERVICE_SOLID_ENTRY_POINT_MAX_DEPTH 500
#define SNAPSHOT_SERVICE_MAX_NUM_MILESTONES_TO_CALC 500

typedef struct snapshots_service_s {
  iota_consensus_conf_t *conf;
  snapshots_provider_t *snapshots_provider;
  milestone_service_t const *milestone_service;
} snapshots_service_t;

/**
 * Initializes a snapshots service
 *
 * @param snapshots_service The service
 * @param snapshots_provider The provider
 * @param milestone_service The milestone service
 * @param conf The configuration
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_init(snapshots_service_t *snapshots_service,
                                      snapshots_provider_t *const snapshots_provider,
                                      milestone_service_t *const milestone_service, iota_consensus_conf_t *conf);

/**
 * Destroys the snapshots service
 *
 * @param snapshots_service The snapshots_service
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_destroy(snapshots_service_t *const snapshots_service);

/**
 * Takes a snapshot and applies it
 *
 * @param snapshots_service The service
 * @param ps The pruning service
 * @param tangle The tangle
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_take_snapshot(snapshots_service_t *const snapshots_service,
                                               pruning_service_t *const ps, tangle_t const *const tangle);

/**
 * Generates a new snapshot
 *
 * @param snapshots_service The service
 * @param target_milestone The new "genesis"
 * @param tangle The tangle
 * @param snapshot The new snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_generate_snapshot(snapshots_service_t *const snapshots_service,
                                                   iota_milestone_t const *const target_milestone,
                                                   tangle_t const *const tangle, snapshot_t *const snapshot);

/**
 * Generates snapshots metadata
 *
 * @param snapshots_service The service
 * @param target_milestone The new "genesis"
 * @param tangle The tangle
 * @param snapshot The new snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_generate_snapshot_metadata(snapshots_service_t *const snapshots_service,
                                                            iota_milestone_t const *const target_milestone,
                                                            tangle_t const *const tangle, snapshot_t *const snapshot);

/**
 * Determines snapshot's new entry point (milestone)
 *
 * @param snapshots_service The service
 * @param entry_point The milestone that will use as new snapshot genesis
 * @param tangle The tangle
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_determine_new_entry_point(snapshots_service_t *const snapshots_service,
                                                           iota_stor_pack_t *const entry_point,
                                                           tangle_t const *const tangle);

/**
 * Persist a snapshot
 *
 * @param snapshots_service The service
 * @param snapshot The snapshot to persist
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_persist_snapshot(snapshots_service_t *const snapshots_service,
                                                  snapshot_t *const snapshot);

/**
 * Generates solid entry points and adds them to snapshot
 *
 * @param snapshots_service The service
 * @param snapshot The new snapshot
 * @param target_milestone The milestone that is the initial entry point
 * @param tangle The tangle
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_update_solid_entry_points(snapshots_service_t *const snapshots_service,
                                                           snapshot_t *const snapshot,
                                                           iota_milestone_t const *const target_milestone,
                                                           tangle_t const *const tangle);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_SERVICE_H__
