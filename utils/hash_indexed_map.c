/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/hash_indexed_map.h"

/*
 * Hash-indexed_hash_set map
 */

bool hash_to_indexed_hash_set_map_contains(hash_to_indexed_hash_set_map_t const *const map,
                                           flex_trit_t const *const hash) {
  hash_to_indexed_hash_set_entry_t *entry = NULL;

  if (!(*map)) {
    return false;
  }

  HASH_FIND(hh, *map, hash, FLEX_TRIT_SIZE_243, entry);

  return entry != NULL;
}

bool hash_to_indexed_hash_set_map_find(hash_to_indexed_hash_set_map_t const *const map, flex_trit_t const *const hash,
                                       hash_to_indexed_hash_set_entry_t **const res) {
  if (map == NULL || (*map) == NULL) {
    return false;
  }
  if (res == NULL) {
    return false;
  }

  HASH_FIND(hh, *map, hash, FLEX_TRIT_SIZE_243, *res);

  return *res != NULL;
}

retcode_t hash_to_indexed_hash_set_map_add_new_set(hash_to_indexed_hash_set_map_t *const map,
                                                   flex_trit_t const *const hash,
                                                   hash_to_indexed_hash_set_entry_t **const new_set_entry,
                                                   size_t const index) {
  *new_set_entry = (hash_to_indexed_hash_set_entry_t *)malloc(sizeof(hash_to_indexed_hash_set_entry_t));
  if (*new_set_entry == NULL) {
    return RC_OOM;
  }

  (*new_set_entry)->approvers = NULL;
  (*new_set_entry)->idx = index;
  memcpy((*new_set_entry)->hash, hash, FLEX_TRIT_SIZE_243);

  HASH_ADD(hh, *map, hash, FLEX_TRIT_SIZE_243, *new_set_entry);

  return RC_OK;
}

void hash_to_indexed_hash_set_map_free(hash_to_indexed_hash_set_map_t *const map) {
  hash_to_indexed_hash_set_entry_t *curr_entry = NULL;
  hash_to_indexed_hash_set_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, *map, curr_entry, tmp_entry) {
    hash243_set_free(&((*map)->approvers));
    HASH_DEL(*map, curr_entry);
    free(curr_entry);
  }
  *map = NULL;
}
