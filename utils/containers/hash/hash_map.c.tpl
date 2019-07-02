/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/hash/hash_{TYPE}_map.h"
#include "utils/handles/rand.h"

retcode_t hash_to_{TYPE}_map_add(hash_to_{TYPE}_map_t *const map, flex_trit_t const *const hash, {TYPE} value) {
  hash_to_{TYPE}_map_entry_t *map_entry = NULL;

  HASH_FIND(hh, *map, hash, FLEX_TRIT_SIZE_243, map_entry);
  if (map_entry == NULL) {
    map_entry = (hash_to_{TYPE}_map_entry_t *)malloc(sizeof(hash_to_{TYPE}_map_entry_t));

    if (map_entry == NULL) {
      return RC_OOM;
    }

    memcpy(map_entry->hash, hash, FLEX_TRIT_SIZE_243);
    map_entry->value = value;
    HASH_ADD(hh, *map, hash, FLEX_TRIT_SIZE_243, map_entry);
  }
  return RC_OK;
}

bool hash_to_{TYPE}_map_contains(hash_to_{TYPE}_map_t const map, flex_trit_t const *const hash) {
  hash_to_{TYPE}_map_entry_t *entry = NULL;

  if (map == NULL) {
    return false;
  }

  HASH_FIND(hh, map, hash, FLEX_TRIT_SIZE_243, entry);
  return entry != NULL;
}

bool hash_to_{TYPE}_map_remove(hash_to_{TYPE}_map_t *const map, flex_trit_t const *const hash) {
  hash_to_{TYPE}_map_entry_t *entry = NULL;

  if (map == NULL || *map == NULL) {
    return false;
  }

  HASH_FIND(hh, *map, hash, FLEX_TRIT_SIZE_243, entry);

  if (entry != NULL) {
    HASH_DEL(*map, entry);
    free(entry);
  }

  return entry != NULL;
}

bool hash_to_{TYPE}_map_find(hash_to_{TYPE}_map_t const map, flex_trit_t const *const hash,
                              hash_to_{TYPE}_map_entry_t **res) {
  if (map == NULL) {
    return false;
  }
  if (res == NULL) {
    return RC_NULL_PARAM;
  }

  HASH_FIND(hh, map, hash, FLEX_TRIT_SIZE_243, *res);
  return *res != NULL;
}
void hash_to_{TYPE}_map_free(hash_to_{TYPE}_map_t *const map) {
  hash_to_{TYPE}_map_entry_t *curr_entry = NULL;
  hash_to_{TYPE}_map_entry_t *tmp_entry = NULL;

  if (map == NULL || *map == NULL) {
    return;
  }

  HASH_ITER(hh, *map, curr_entry, tmp_entry) {
    HASH_DEL(*map, curr_entry);
    free(curr_entry);
  }
  *map = NULL;
}

void hash_to_{TYPE}_map_keys(hash_to_{TYPE}_map_t *const map, hash243_set_t *const keys) {
  hash_to_{TYPE}_map_entry_t *curr_entry = NULL;
  hash_to_{TYPE}_map_entry_t *tmp_entry = NULL;
  HASH_ITER(hh, *map, curr_entry, tmp_entry) { hash243_set_add(keys, curr_entry->hash); }
}

retcode_t hash_to_{TYPE}_map_copy(hash_to_{TYPE}_map_t const *const src, hash_to_{TYPE}_map_t *const dst) {
  retcode_t ret;
  hash_to_{TYPE}_map_entry_t *curr_entry = NULL;
  hash_to_{TYPE}_map_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, *src, curr_entry, tmp_entry) {
    ret = hash_to_{TYPE}_map_add(dst, curr_entry->hash, curr_entry->value);
    if (ret != RC_OK) {
      return ret;
    }
  }

  return RC_OK;
}

size_t hash_to_{TYPE}_map_size(hash_to_{TYPE}_map_t const map) { return HASH_COUNT(map); }

bool hash_to_{TYPE}_map_equal(hash_to_{TYPE}_map_t const lhs, hash_to_{TYPE}_map_t const rhs) {
  if (HASH_COUNT(lhs) != HASH_COUNT(rhs)) {
    return false;
  }

  hash_to_{TYPE}_map_entry_t *rhs_entry = NULL;
  hash_to_{TYPE}_map_entry_t *lhs_entry = NULL;
  hash_to_{TYPE}_map_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, lhs, lhs_entry, tmp_entry) {
    if (!hash_to_{TYPE}_map_find(rhs, lhs_entry->hash, &rhs_entry)) {
      return false;
    }
    if (lhs_entry->value != rhs_entry->value) {
      return false;
    }
  }

  return true;
}