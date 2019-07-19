/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_PRUNING_SERVICE_H__
#define __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_PRUNING_SERVICE_H__

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

typedef struct pruning_service_s {
  volatile bool running;
  thread_handle_t pruning_service_thread;
  cond_handle_t cond_pruning_service;
  iota_consensus_conf_t const *conf;
  uint64_t last_pruned_snapshot_index;
  uint64_t last_snapshot_index_to_prune;
  rw_lock_handle_t rw_lock;
  snapshot_t new_snapshot;
  spent_addresses_service_t *spent_addresses_service;
  tips_cache_t *tips_cache;
} pruning_service_t;

/**
 * Initializes a pruning service
 *
 * @param[out] ps The pruning service
 * @param[in] snapshot_provider The snapshots provider
 * @param[in, out] spent_addresses_service The spent addresses service
 * @param[out] tips_cache The tips cache
 * @param[in] conf The consensus's conf
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_pruning_service_init(pruning_service_t *const ps,
                                                    snapshots_provider_t *const snapshot_provider,
                                                    spent_addresses_service_t *const spent_addresses_service,
                                                    tips_cache_t *const tips_cache,
                                                    iota_consensus_conf_t const *const conf);

/**
 * Starts a pruning service
 *
 * @param[in, out] ps The pruning service
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_pruning_service_start(pruning_service_t *const ps);

/**
 * Stops a local snapshots service
 *
 * @param[in, out] ps The pruning service
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_pruning_service_stop(pruning_service_t *const ps);

/**
 * Destroys a pruning service
 *
 * @param[in, out] ps The pruning service
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_pruning_service_destroy(pruning_service_t *const ps);

/**
 * Updates the last snapshot index so all transactions before it should be pruned
 *
 * @param[out] ps The pruning service
 * @param[in] snapshot The last snapshot
 *
 * @return void
 */
void iota_local_snapshots_pruning_service_update_current_snapshot(pruning_service_t *const ps,
                                                                  snapshot_t *const snapshot);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_PRUNING_SERVICE_H__
