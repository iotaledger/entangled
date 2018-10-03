/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_STATE_DIFF_H__
#define __CONSENSUS_SNAPSHOT_STATE_DIFF_H__

#include "uthash.h"

#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct state_entry_t {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  int64_t value;
  UT_hash_handle hh;
} state_entry_t;

typedef state_entry_t *state_map_t;

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_STATE_DIFF_H__
