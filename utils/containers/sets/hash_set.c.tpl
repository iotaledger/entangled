/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/sets/hash$SIZE_set.h"

uint32_t hash$SIZE_set_size(hash$SIZE_set_t const *const set) {
  return HASH_COUNT(*set);
}

retcode_t hash$SIZE_set_add(hash$SIZE_set_t *const set,
                            flex_trit_t const *const hash) {
  hash$SIZE_set_entry_t *entry = NULL;

  if ((entry = malloc(sizeof(hash$SIZE_set_entry_t))) == NULL) {
    return RC_UTILS_OOM;
  }
  memcpy(entry->hash, hash, FLEX_TRIT_SIZE_$SIZE);
  HASH_ADD(hh, *set, hash, FLEX_TRIT_SIZE_$SIZE, entry);
  return RC_OK;
}

retcode_t hash$SIZE_set_append(hash$SIZE_set_t const *const set1,
                               hash$SIZE_set_t *const set2) {
  retcode_t ret = RC_OK;
  hash$SIZE_set_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *set1, iter, tmp) {
    if (!hash$SIZE_set_contains(set2, iter->hash)) {
      if ((ret = hash$SIZE_set_add(set2, iter->hash)) != RC_OK) {
        return ret;
      }
    }
  }
  return ret;
}

bool hash$SIZE_set_contains(hash$SIZE_set_t const *const set,
                            flex_trit_t const *const hash) {
  hash$SIZE_set_entry_t *entry = NULL;

  if (*set == NULL) {
    return false;
  }

  HASH_FIND(hh, *set, hash, FLEX_TRIT_SIZE_$SIZE, entry);
  return entry != NULL;
}

void hash$SIZE_set_free(hash$SIZE_set_t *const set) {
  hash$SIZE_set_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *set, iter, tmp) {
    HASH_DEL(*set, iter);
    free(iter);
  }
  *set = NULL;
}

retcode_t hash$SIZE_set_for_each(hash$SIZE_set_t const *const set,
                                 hash$SIZE_on_container_func func,
                                 void *const container) {
  retcode_t ret = RC_OK;
  hash$SIZE_set_entry_t *curr_entry = NULL;
  hash$SIZE_set_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, *set, curr_entry, tmp_entry) {
    if ((ret = func(container, curr_entry->hash)) != RC_OK) {
      return ret;
    }
  }
  return ret;
}
