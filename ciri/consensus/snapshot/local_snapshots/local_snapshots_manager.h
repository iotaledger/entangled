/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_MANAGER_H__
#define __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_MANAGER_H__

#include <stdbool.h>
#include <stdint.h>

#include "ciri/consensus/conf.h"
#include "ciri/consensus/milestone/milestone_tracker.h"
#include "ciri/consensus/snapshot/local_snapshots/pruning_service.h"
#include "ciri/consensus/snapshot/snapshots_provider.h"
#include "ciri/consensus/snapshot/snapshots_service.h"
#include "ciri/consensus/spent_addresses/spent_addresses_service.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/node/tips_cache.h"
#include "utils/handles/cond.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct local_snapshots_manager_s {
  bool running;
  iota_consensus_conf_t *conf;
  thread_handle_t local_snapshots_thread;
  cond_handle_t cond_local_snapshots;
  milestone_tracker_t const *mt;
  snapshots_service_t *snapshots_service;
  uint64_t last_snapshot_transactions_count;
  pruning_service_t ps;
} local_snapshots_manager_t;

/**
 * Initializes a local snapshots manager
 *
 * @param lsm The local snapshots manager
 * @param snapshots_service The snapshots service
 * @param conf Consensus configuration
 * @param mt The milestones tracker
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_manager_init(local_snapshots_manager_t *lsm,
                                            snapshots_service_t *const snapshots_service,
                                            iota_consensus_conf_t *const conf, milestone_tracker_t const *const mt,
                                            spent_addresses_service_t *const spent_addresses_service,
                                            tips_cache_t *const tips_cache);

/**
 * Starts a local snapshots manager
 *
 * @param lsm The local snapshots manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_manager_start(local_snapshots_manager_t *const lsm);

/**
 * Stops a local snapshots manager
 *
 * @param lsm The local snapshots manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_manager_stop(local_snapshots_manager_t *const lsm);

/**
 * Destroys a local snapshots manager
 *
 * @param lsm The local snapshots manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_manager_destroy(local_snapshots_manager_t *const lsm);

/**
 * Determines if a snapshot should be taken
 *
 * @param lsm The local snapshots manager
 * @param tangle The Tangle
 *
 * @return True is snapshot should be taken
 */
bool iota_local_snapshots_manager_should_take_snapshot(local_snapshots_manager_t const *const lsm,
                                                       tangle_t const *const tangle);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_MANAGER_H__
