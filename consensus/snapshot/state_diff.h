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

retcode_t state_diff_destroy(state_diff_t *const state);
bool state_diff_is_consistent(state_diff_t *const state);
size_t state_diff_serialized_size(state_diff_t const *const diff);
retcode_t state_diff_serialize(state_diff_t const *const diff,
                               byte_t *const bytes);
retcode_t state_diff_deserialize(byte_t const *const bytes, size_t size,
                                 state_diff_t *const diff);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SNAPSHOT_STATE_DIFF_H__
