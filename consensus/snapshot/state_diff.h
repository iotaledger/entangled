/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_STATE_DIFF_H__
#define __CONSENSUS_SNAPSHOT_STATE_DIFF_H__

#include "uthash.h"

#include "common/errors.h"
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

size_t iota_state_diff_serialized_size(state_map_t const *const state_diff);
retcode_t iota_state_diff_serialize(state_map_t const *const state_diff,
                                    byte_t *const bytes);
retcode_t iota_state_diff_deserialize(byte_t const *const bytes, size_t size,
                                      state_map_t *const state_diff);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_STATE_DIFF_H__
