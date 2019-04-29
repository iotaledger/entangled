/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_SERVICE_H__
#define __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_SERVICE_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "consensus/milestone/milestone_service.h"
#include "consensus/milestone/milestone_tracker.h"
#include "consensus/snapshot/snapshots_provider.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct snapshots_service_s {
  iota_consensus_conf_t *conf;
  snapshots_provider_t *snapshots_provider;
  milestone_service_t const *milestone_service;
  tangle_t tangle;
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
 * @param milestone_tracker The milestone tracker
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_take_snapshot(snapshots_service_t *const snapshots_service,
                                               milestone_tracker_t const *const milestone_tracker);

/**
 * Generates a new snapshot
 *
 * @param snapshots_service The service
 * @param target_milestone The new "genesis"
 * @param snapshot The new snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_generate_snapshot(snapshots_service_t *const snapshots_service,
                                                   iota_milestone_t const *const target_milestone,
                                                   snapshot_t *const snapshot);

/**
 * Generates snapshots metadata
 *
 * @param snapshots_service The service
 * @param target_milestone The new "genesis"
 * @param snapshot The new snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_generate_snapshot_metadata(snapshots_service_t *const snapshots_service,
                                                            iota_milestone_t const *const target_milestone,
                                                            snapshot_t *const snapshot);

/**
 * Determines snapshot's new entry point (milestone)
 *
 * @param snapshots_service The service
 * @param entry_point The milestone that will use as new snapshot genesis
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_determine_new_entry_point(snapshots_service_t *const snapshots_service,
                                                           iota_stor_pack_t *const entry_point);

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
 *
 * @return a status code
 */
retcode_t iota_snapshots_service_update_solid_entry_points(snapshots_service_t *const snapshots_service,
                                                           snapshot_t *const snapshot,
                                                           iota_milestone_t const *const target_milestone);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_SERVICE_H__
