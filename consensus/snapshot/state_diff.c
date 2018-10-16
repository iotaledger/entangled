/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/state_diff.h"

int64_t state_diff_sum(state_diff_t const *const state) {
  int64_t sum = 0;
  state_diff_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *state, iter, tmp) { sum += iter->value; }
  return sum;
}

retcode_t state_diff_add_or_sum(state_diff_t *const state,
                                flex_trit_t const *const hash,
                                int64_t const value) {
  retcode_t ret = RC_OK;
  state_diff_entry_t *entry = NULL;

  state_diff_find(*state, hash, entry);
  if (entry) {
    entry->value += value;
  } else {
    if ((ret = state_diff_add(state, hash, value)) != RC_OK) {
      return ret;
    }
  }
  return RC_OK;
}

retcode_t state_diff_add_or_replace(state_diff_t *const state,
                                    flex_trit_t const *const hash,
                                    int64_t const value) {
  retcode_t ret = RC_OK;
  state_diff_entry_t *entry = NULL;

  state_diff_find(*state, hash, entry);
  if (entry) {
    entry->value = value;
  } else {
    if ((ret = state_diff_add(state, hash, value)) != RC_OK) {
      return ret;
    }
  }
  return RC_OK;
}

retcode_t state_diff_create_patch(state_diff_t const *const state,
                                  state_diff_t const *const diff,
                                  state_diff_t *const patch) {
  retcode_t ret = RC_OK;
  state_diff_entry_t *iter = NULL, *entry = NULL, *tmp = NULL;

  HASH_ITER(hh, *diff, iter, tmp) {
    state_diff_find(*state, iter->hash, entry);
    if ((ret = state_diff_add(patch, iter->hash,
                              (entry ? entry->value : 0) + iter->value)) !=
        RC_OK) {
      return ret;
    }
  }
  return ret;
}

retcode_t state_diff_apply_patch(state_diff_t *const state,
                                 state_diff_t const *const patch) {
  retcode_t ret = RC_OK;
  state_diff_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *patch, iter, tmp) {
    if ((ret = state_diff_add_or_sum(state, iter->hash, iter->value)) !=
        RC_OK) {
      return ret;
    }
  }
  return ret;
}

retcode_t state_diff_merge_patch(state_diff_t *const state,
                                 state_diff_t const *const patch) {
  retcode_t ret = RC_OK;
  state_diff_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *patch, iter, tmp) {
    if ((ret = state_diff_add_or_replace(state, iter->hash, iter->value)) !=
        RC_OK) {
      return ret;
    }
  }
  return ret;
}

bool state_diff_is_consistent(state_diff_t *const diff) {
  state_diff_entry_t *entry, *tmp;

  HASH_ITER(hh, *diff, entry, tmp) {
    if (entry->value <= 0) {
      if (entry->value < 0) {
        return false;
      }
      HASH_DEL(*diff, entry);
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

retcode_t state_diff_deserialize(byte_t const *const bytes, size_t const size,
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
