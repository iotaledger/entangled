/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_MANAGER_H__
#define __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_MANAGER_H__

#include <stdbool.h>
#include <stdint.h>

#include "consensus/conf.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "utils/handles/cond.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct local_snapshots_manager_s {
  bool running;
  iota_consensus_conf_t *conf;
  thread_handle_t local_snapshots_thread;
  cond_handle_t cond_local_snapshots;
  milestone_tracker_t const *mt;
  snapshots_provider_t const *snapshots_provider;

  // Muteable data
  tangle_t tangle;
  size_t last_snapshot_transactions_count;
} local_snapshots_manager_t;

/**
 * Initializes a local snapshots manager
 *
 * @param lsm The local snapshots manager
 * @param conf Consensus configuration
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_manager_init(local_snapshots_manager_t *lsm,
                                            snapshots_provider_t *const snapshots_provider,
                                            iota_consensus_conf_t *const conf, milestone_tracker_t const *const mt);

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

bool iota_local_snapshots_manager_should_take_snapshot(local_snapshots_manager_t const *const lsm);

retcode_t iota_local_snapshots_manager_take_snapshot(local_snapshots_manager_t *const lsm);

retcode_t iota_local_snapshots_manager_determine_new_entry_point(local_snapshots_manager_t *const lsm,
                                                                 iota_stor_pack_t *const entry_point);


#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_H__
