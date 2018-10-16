/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/state_diff.h"

retcode_t state_diff_destroy(state_diff_t *const state) {
  state_diff_entry_t *entry, *tmp;

  if (state == NULL) {
    return RC_SNAPSHOT_NULL_STATE;
  } else if (*state == NULL) {
    return RC_OK;
  }

  HASH_ITER(hh, *state, entry, tmp) {
    HASH_DEL(*state, entry);
    free(entry);
  }
  return RC_OK;
}

bool state_diff_is_consistent(state_diff_t *const state) {
  state_diff_entry_t *entry, *tmp;

  HASH_ITER(hh, *state, entry, tmp) {
    if (entry->value <= 0) {
      if (entry->value < 0) {
        return false;
      }
      HASH_DEL(*state, entry);
      free(entry);
    }
  }
  return true;
}

size_t state_diff_serialized_size(state_diff_t const *const diff) {
  if (diff == NULL) {
    return 0;
  }
  return HASH_COUNT(*diff) * (FLEX_TRIT_SIZE_243 + sizeof(int64_t));
}

retcode_t state_diff_serialize(state_diff_t const *const diff,
                               byte_t *const bytes) {
  uint64_t offset = 0;
  state_diff_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *diff, iter, tmp) {
    memcpy(bytes + offset, iter->hash, FLEX_TRIT_SIZE_243);
    offset += FLEX_TRIT_SIZE_243;
    memcpy(bytes + offset, &iter->value, sizeof(int64_t));
    offset += sizeof(int64_t);
  }
  return RC_OK;
}

retcode_t state_diff_deserialize(byte_t const *const bytes, size_t size,
                                 state_diff_t *const diff) {
  uint64_t offset = 0;
  state_diff_entry_t *new = NULL;

  for (size_t i = 0; i < size / (FLEX_TRIT_SIZE_243 + sizeof(int64_t)); i++) {
    if ((new = malloc(sizeof(state_diff_entry_t))) == NULL) {
      return RC_SNAPSHOT_OOM;
    }
    memcpy(new->hash, bytes + offset, FLEX_TRIT_SIZE_243);
    offset += FLEX_TRIT_SIZE_243;
    memcpy(&new->value, bytes + offset, sizeof(int64_t));
    offset += sizeof(int64_t);
    HASH_ADD(hh, *diff, hash, FLEX_TRIT_SIZE_243, new);
  }
  return RC_OK;
}
