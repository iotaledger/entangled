/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_PRUNING_MANAGER_H__
#define __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_PRUNING_MANAGER_H__

#include <stdbool.h>
#include <stdint.h>

#include "ciri/consensus/conf.h"
#include "ciri/consensus/snapshot/snapshots_provider.h"
#include "ciri/consensus/spent_addresses/spent_addresses_service.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/node/tips_cache.h"
#include "utils/handles/cond.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pruning_manager_s {
  bool running;
  thread_handle_t pruning_manager_thread;
  cond_handle_t cond_pruning_manager;
  iota_consensus_conf_t *conf;
  uint64_t last_pruned_snapshot_index;
  uint64_t last_snapshot_index_to_prune;
  lock_handle_t lock_handle;
  snapshots_provider_t *snapshot_provider;
  snapshot_t *new_snapshot;
  spent_addresses_service_t *spent_addresses_service;
  tips_cache_t *tips_cache;
} pruning_manager_t;

/**
 * Initializes a pruning manager
 *
 * @param pm The pruning manager
 * @param snapshot_provider The snapshots provider
 * @param conf The consensus's conf
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_pruning_manager_init(pruning_manager_t *const pm,
                                                    snapshots_provider_t *const snapshot_provider,
                                                    spent_addresses_service_t *const spent_addresses_service,
                                                    tips_cache_t *const tips_cache, iota_consensus_conf_t *const conf);

/**
 * Starts a pruning manager
 *
 * @param pm The pruning manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_pruning_manager_start(pruning_manager_t *const pm);

/**
 * Stops a local snapshots manager
 *
 * @param pm The pruning manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_pruning_manager_stop(pruning_manager_t *const pm);

/**
 * Destroys a pruning manager
 *
 * @param pm The pruning manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_pruning_manager_destroy(pruning_manager_t *const pm);

/**
 * Updates the last snapshot index so all transactions before it should be pruned
 *
 * @param pm The pruning manager
 * @param snapshot The last snapshot
 *
 * @return void
 */
void iota_local_snapshots_pruning_manager_update_current_snapshot(pruning_manager_t *const pm,
                                                                  snapshot_t *const snapshot);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_PRUNING_MANAGER_H__
