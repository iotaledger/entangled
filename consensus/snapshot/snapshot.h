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
#include "common/model/transaction.h"
#include "common/trinary/trit_array.h"
#include "utils/handles/rw_lock.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SNAPSHOT_PUBKEY                                                        \
  "TTXJUGKTNPOOEXSTQVVACENJOQUROXYKDRCVK9LHUXILCLABLGJTIPNF9REWHOIMEUKWQLUOKD" \
  "9CZUYAC"
#define SNAPSHOT_PUBKEY_DEPTH 6
#define SNAPSHOT_INDEX 4
#define SPENT_ADDRESSES_INDEX 5

typedef struct state_entry_t {
  trit_array_p hash;
  int64_t value;
  UT_hash_handle hh;
} state_entry_t;

typedef state_entry_t *state_map_t;

typedef struct snapshot_t {
  rw_lock_handle_t rw_lock;
  state_map_t state;
} snapshot_t;

extern retcode_t iota_snapshot_init();
extern retcode_t iota_snapshot_apply();

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_SNAPSHOT_H__
