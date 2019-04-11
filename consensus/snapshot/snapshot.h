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
  uint64_t index;
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
retcode_t iota_snapshot_init(snapshot_t *const snapshot, iota_consensus_conf_t *const conf);

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
uint64_t iota_snapshot_get_index(snapshot_t *const snapshot);

/**
 * Gets the balance of a given address hash
 *
 * @param snapshot The snapshot
 * @param hash The address hash
 *
 * @return a status code
 */
retcode_t iota_snapshot_get_balance(snapshot_t *const snapshot, flex_trit_t *const hash, int64_t *balance);

/**
 * Creates a patch of a snapshot state and a delta
 *
 * @param snapshot The snapshot
 * @param delta The delta
 * @param patch The patch
 *
 * @return a status code
 */
retcode_t iota_snapshot_create_patch(snapshot_t *const snapshot, state_delta_t *const delta,
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
retcode_t iota_snapshot_apply_patch(snapshot_t *const snapshot, state_delta_t *const patch, uint64_t index);

/**
 * Coppies a snapshot struct
 *
 * @param src The source snapshot
 * @param dst The destination snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshot_copy(snapshot_t const *const src, snapshot_t *const dst);

/**
 * Locks snapshot for write
 *
 * @param snapshot The snapshot
 *
 * @return void
 */
void iota_snapshot_lock_write(snapshot_t *const snapshot);

/**
 * Locks snapshot for read
 *
 * @param snapshot The snapshot
 *
 * @return void
 */
void iota_snapshot_lock_read(snapshot_t *const snapshot);

/**
 * Unlocks snapshot
 *
 * @param snapshot The snapshot
 *
 * @return void
 */
void iota_snapshot_unlock(snapshot_t *const snapshot);

/**
 * Reads a snapshot from file
 *
 * @param snapshot The snapshot
 * @param snapshot_file The file with serialized snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshot_read_from_file(snapshot_t *const snapshot, char const *const snapshot_file);

/**
 * Writes a snapshot to file
 *
 * @param snapshot The snapshot
 * @param snapshot_file The file with serialized snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshot_write_to_file(snapshot_t const *const snapshot, char const *const snapshot_file);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_SNAPSHOT_H__
