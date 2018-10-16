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
#include "consensus/defs.h"
#include "consensus/snapshot/state_diff.h"
#include "utils/handles/rw_lock.h"

#define SNAPSHOT_PUBKEY                                                        \
  "TTXJUGKTNPOOEXSTQVVACENJOQUROXYKDRCVK9LHUXILCLABLGJTIPNF9REWHOIMEUKWQLUOKD" \
  "9CZUYAC"
#define SNAPSHOT_PUBKEY_DEPTH 6UL
#define SNAPSHOT_INDEX 4UL
#define SNAPSHOT_TIMESTAMP 1537203600UL

#ifdef __cplusplus
extern "C" {
#endif

typedef struct snapshot_s {
  rw_lock_handle_t rw_lock;
  size_t index;
  state_diff_t state;
} snapshot_t;

/**
 * Initializes a snapshot
 *
 * @param snapshot The snapshot
 * @param snapshot_file The snapshot file path
 * @param snapshot_sig_file The snapshot signature file path
 * @param testnet Whether the node runs on testnet or not
 *
 * @return a status code
 */
retcode_t iota_snapshot_init(snapshot_t *const snapshot,
                             char const *const snapshot_file,
                             char const *const snapshot_sig_file, bool testnet);

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
 * Creates a patch of a snapshot state and a diff
 *
 * @param snapshot The snapshot
 * @param diff The diff
 * @param patch The patch
 *
 * @return a status code
 */
retcode_t iota_snapshot_create_patch(snapshot_t *const snapshot,
                                     state_diff_t *const diff,
                                     state_diff_t *const patch);

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
                                    state_diff_t *const patch, size_t index);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_SNAPSHOT_H__
