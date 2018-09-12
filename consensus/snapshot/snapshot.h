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

#include "uthash.h"

#include "common/errors.h"
#include "common/trinary/trit_array.h"
#include "utils/handles/rw_lock.h"

#define SNAPSHOT_PUBKEY                                                        \
  "TTXJUGKTNPOOEXSTQVVACENJOQUROXYKDRCVK9LHUXILCLABLGJTIPNF9REWHOIMEUKWQLUOKD" \
  "9CZUYAC"
#define SNAPSHOT_PUBKEY_DEPTH 6
#define SNAPSHOT_INDEX 4
#define SPENT_ADDRESSES_INDEX 5
#define IOTA_SUPPLY 2779530283277761L

#ifdef __cplusplus
extern "C" {
#endif

typedef struct state_entry_t {
  // TODO trit array
  tryte_t hash[81];
  int64_t value;
  UT_hash_handle hh;
} state_entry_t;

typedef state_entry_t *state_map_t;

typedef struct snapshot_t {
  rw_lock_handle_t rw_lock;
  size_t index;
  state_map_t state;
} snapshot_t;

/**
 * Initializes a snapshot
 *
 * @param snapshot The snapshot
 * @param testnet Whether the node runs on testnet or not
 *
 * @return a status code
 */
extern retcode_t iota_snapshot_init(snapshot_t *const snapshot, bool testnet);

/**
 * Destroys a snapshot
 *
 * @param snapshot The snapshot
 *
 * @return a status code
 */
extern retcode_t iota_snapshot_destroy(snapshot_t *const snapshot);

/**
 * Checks if a given state is consistent
 *
 * @param state The state
 *
 * @return true if consistent, false otherwise
 */
extern bool iota_snapshot_is_state_consistent(state_map_t *const state);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_SNAPSHOT_H__
