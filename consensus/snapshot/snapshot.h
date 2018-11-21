/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_SNAPSHOT_H__
#define __CONSENSUS_SNAPSHOT_SNAPSHOT_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/trinary/trit_array.h"
#include "consensus/conf.h"
#include "consensus/snapshot/state_delta.h"
#include "utils/handles/rw_lock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct snapshot_s {
  iota_consensus_conf_t *conf;
  rw_lock_handle_t rw_lock;
  size_t index;
  state_delta_t state;
} snapshot_t;

/**
 * Initializes a snapshot
 *
 * @param snapshot The snapshot
 * @param conf Consensus configuration
 *
 * @return a status code
 */
retcode_t iota_snapshot_init(snapshot_t *const snapshot,
                             iota_consensus_conf_t *const conf);

/**
 * Destroys a snapshot
 *
 * @param snapshot The snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshot_destroy(snapshot_t *const snapshot);

/**
 * Gets the index of a snapshot
 *
 * @param snapshot The snapshot
 *
 * @return the snapshot index
 */
size_t iota_snapshot_get_index(snapshot_t *const snapshot);

/**
 * Gets the balance of a given address hash
 *
 * @param snapshot The snapshot
 * @param hash The address hash
 *
 * @return a status code
 */
retcode_t iota_snapshot_get_balance(snapshot_t *const snapshot,
                                    flex_trit_t *const hash, int64_t *balance);

/**
 * Creates a patch of a snapshot state and a delta
 *
 * @param snapshot The snapshot
 * @param delta The delta
 * @param patch The patch
 *
 * @return a status code
 */
retcode_t iota_snapshot_create_patch(snapshot_t *const snapshot,
                                     state_delta_t *const delta,
                                     state_delta_t *const patch);

/**
 * Applies a patch to a snapshot state
 *
 * @param snapshot The snapshot
 * @param patch The patch
 * @param index A new index for the snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshot_apply_patch(snapshot_t *const snapshot,
                                    state_delta_t *const patch, size_t index);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_SNAPSHOT_H__
