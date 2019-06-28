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

#include "ciri/consensus/conf.h"
#include "ciri/consensus/snapshot/snapshot_metadata.h"
#include "ciri/consensus/snapshot/state_delta.h"
#include "common/errors.h"
#include "common/trinary/trit_array.h"
#include "utils/handles/rw_lock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct snapshot_s {
  iota_consensus_conf_t *conf;
  rw_lock_handle_t rw_lock;
  state_delta_t state;
  snapshot_metadata_t metadata;
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
 * Resets a snapshot
 *
 * @param snapshot The snapshot
 * @param conf Consensus configuration
 *
 * @return a status code
 */
retcode_t iota_snapshot_reset(snapshot_t *const snapshot, iota_consensus_conf_t *const conf);

/**
 * Builds a snapshot from a snapshot file
 *
 * @param snapshot The snapshot
 * @param conf Consensus configuration
 *
 * @return a status code
 */
retcode_t iota_snapshot_load_built_in_snapshot(snapshot_t *const snapshot, iota_consensus_conf_t *const conf);

/**
 * Loads snapshot from the local snapshot file
 *
 * @param snapshot The snapshot
 * @param conf Consensus configuration
 *
 * @return a status code
 */
retcode_t iota_snapshot_load_local_snapshot(snapshot_t *const snapshot, iota_consensus_conf_t *const conf);

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
 * Applies a patch to a snapshot state without taking a lock
 *
 * @param snapshot The snapshot
 * @param patch The patch
 * @param index A new index for the snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshot_apply_patch_no_lock(snapshot_t *const snapshot, state_delta_t *const patch, uint64_t index);

/**
 * Copies a snapshot struct
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
static inline void iota_snapshot_write_lock(snapshot_t *const snapshot) { rw_lock_handle_wrlock(&snapshot->rw_lock); }

/**
 * Locks snapshot for read
 *
 * @param snapshot The snapshot
 *
 * @return void
 */
static inline void iota_snapshot_read_lock(snapshot_t *const snapshot) { rw_lock_handle_rdlock(&snapshot->rw_lock); }

/**
 * Unlocks snapshot
 *
 * @param snapshot The snapshot
 *
 * @return void
 */
static inline void iota_snapshot_unlock(snapshot_t *const snapshot) { rw_lock_handle_unlock(&snapshot->rw_lock); }

/**
 * Reads a snapshot from file
 *
 * @param snapshot The snapshot
 * @param snapshot_file The file with serialized snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshot_state_read_from_file(snapshot_t *const snapshot, char const *const snapshot_file);

/**
 * Writes a snapshot to file
 *
 * @param snapshot The snapshot
 * @param snapshot_file_base The file with serialized snapshot
 *
 * @return a status code
 */
retcode_t iota_snapshot_write_to_file(snapshot_t const *const snapshot, char const *const snapshot_file_base);

/**
 * Sets solid entry points
 *
 * @param snapshot The snapshot
 * @param keys The snapshot solid entry points hashes
 *
 * @return void
 */
void iota_snapshot_solid_entry_points_set(snapshot_t *const snapshot, hash243_set_t *const keys);

/**
 * Checks if hash is a solid entry point
 *
 * @param snapshot The snapshot
 * @param hash The hash in question
 *
 * @return True if hash is a solid entry point
 */
bool iota_snapshot_has_solid_entry_point(snapshot_t *const snapshot, flex_trit_t const *const hash);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_SNAPSHOT_H__
