/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/state_delta.h"

int64_t state_delta_sum(state_delta_t const *const state) {
  int64_t sum = 0;
  state_delta_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *state, iter, tmp) { sum += iter->value; }
  return sum;
}

retcode_t state_delta_add_or_sum(state_delta_t *const state,
                                 flex_trit_t const *const hash,
                                 int64_t const value) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *entry = NULL;

  state_delta_find(*state, hash, entry);
  if (entry) {
    entry->value += value;
  } else {
    if ((ret = state_delta_add(state, hash, value)) != RC_OK) {
      return ret;
    }
  }
  return RC_OK;
}

retcode_t state_delta_add_or_replace(state_delta_t *const state,
                                     flex_trit_t const *const hash,
                                     int64_t const value) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *entry = NULL;

  state_delta_find(*state, hash, entry);
  if (entry) {
    entry->value = value;
  } else {
    if ((ret = state_delta_add(state, hash, value)) != RC_OK) {
      return ret;
    }
  }
  return RC_OK;
}

retcode_t state_delta_create_patch(state_delta_t const *const state,
                                   state_delta_t const *const delta,
                                   state_delta_t *const patch) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *iter = NULL, *entry = NULL, *tmp = NULL;

  HASH_ITER(hh, *delta, iter, tmp) {
    state_delta_find(*state, iter->hash, entry);
    if ((ret = state_delta_add(patch, iter->hash,
                               (entry ? entry->value : 0) + iter->value)) !=
        RC_OK) {
      return ret;
    }
  }
  return ret;
}

retcode_t state_delta_apply_patch(state_delta_t *const state,
                                  state_delta_t const *const patch) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *patch, iter, tmp) {
    if ((ret = state_delta_add_or_sum(state, iter->hash, iter->value)) !=
        RC_OK) {
      return ret;
    }
  }
  return ret;
}

retcode_t state_delta_merge_patch(state_delta_t *const state,
                                  state_delta_t const *const patch) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *patch, iter, tmp) {
    if ((ret = state_delta_add_or_replace(state, iter->hash, iter->value)) !=
        RC_OK) {
      return ret;
    }
  }
  return ret;
}

bool state_delta_is_consistent(state_delta_t *const delta) {
  state_delta_entry_t *entry, *tmp;

  HASH_ITER(hh, *delta, entry, tmp) {
    if (entry->value <= 0) {
      if (entry->value < 0) {
        return false;
      }
      HASH_DEL(*delta, entry);
      free(entry);
    }
  }
  return true;
}

size_t state_delta_serialized_size(state_delta_t const *const delta) {
  if (delta == NULL) {
    return 0;
  }
  return HASH_COUNT(*delta) * (FLEX_TRIT_SIZE_243 + sizeof(int64_t));
}

retcode_t state_delta_serialize(state_delta_t const *const delta,
                                byte_t *const bytes) {
  uint64_t offset = 0;
  state_delta_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *delta, iter, tmp) {
    memcpy(bytes + offset, iter->hash, FLEX_TRIT_SIZE_243);
    offset += FLEX_TRIT_SIZE_243;
    memcpy(bytes + offset, &iter->value, sizeof(int64_t));
    offset += sizeof(int64_t);
  }
  return RC_OK;
}

retcode_t state_delta_deserialize(byte_t const *const bytes, size_t const size,
                                  state_delta_t *const delta) {
  uint64_t offset = 0;
  state_delta_entry_t *new = NULL;

  for (size_t i = 0; i < size / (FLEX_TRIT_SIZE_243 + sizeof(int64_t)); i++) {
    if ((new = malloc(sizeof(state_delta_entry_t))) == NULL) {
      return RC_SNAPSHOT_OOM;
    }
    memcpy(new->hash, bytes + offset, FLEX_TRIT_SIZE_243);
    offset += FLEX_TRIT_SIZE_243;
    memcpy(&new->value, bytes + offset, sizeof(int64_t));
    offset += sizeof(int64_t);
    HASH_ADD(hh, *delta, hash, FLEX_TRIT_SIZE_243, new);
  }
  return RC_OK;
}
