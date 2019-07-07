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

#include "ciri/consensus/tangle/tangle.h"
#include "utils/handles/cond.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pruning_manager_s {
  bool running;
  thread_handle_t pruning_manager_thread;
  cond_handle_t cond_pruning_manager;
} pruning_manager_t;

/**
 * Initializes a pruning manager
 *
 * @param pm The pruning manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_pruning_manager_init(pruning_manager_t *pm);

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

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_LOCAL_SNAPSHOTS_PRUNING_MANAGER_H__
