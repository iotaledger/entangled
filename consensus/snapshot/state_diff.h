/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SNAPSHOT_STATE_DIFF_H__
#define __CONSENSUS_SNAPSHOT_STATE_DIFF_H__

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utils/hash_containers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef hash_to_int_map_entry_t state_diff_entry_t;
typedef state_diff_entry_t *state_diff_t;

#define state_diff_add(diff, hash, value) hash_int_map_add(diff, hash, value)
#define state_diff_find(diff, hash, entry) \
  HASH_FIND(hh, diff, hash, FLEX_TRIT_SIZE_243, entry);
#define state_diff_destroy(diff) hash_int_map_free(diff)

int64_t state_diff_sum(state_diff_t const *const state);
retcode_t state_diff_add_or_sum(state_diff_t *const state,
                                flex_trit_t const *const hash,
                                int64_t const value);
retcode_t state_diff_add_or_replace(state_diff_t *const state,
                                    flex_trit_t const *const hash,
                                    int64_t const value);
retcode_t state_diff_create_patch(state_diff_t const *const state,
                                  state_diff_t const *const diff,
                                  state_diff_t *const patch);
retcode_t state_diff_apply_patch(state_diff_t *const state,
                                 state_diff_t const *const patch);
retcode_t state_diff_merge_patch(state_diff_t *const state,
                                 state_diff_t const *const patch);
bool state_diff_is_consistent(state_diff_t *const diff);

size_t state_diff_serialized_size(state_diff_t const *const diff);
retcode_t state_diff_serialize(state_diff_t const *const diff,
                               byte_t *const bytes);
retcode_t state_diff_deserialize(byte_t const *const bytes, size_t const size,
                                 state_diff_t *const diff);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_STATE_DIFF_H__
