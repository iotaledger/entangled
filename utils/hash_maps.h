/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HASH_CONTAINERS_H__
#define __UTILS_HASH_CONTAINERS_H__

#include "uthash.h"
#include "utlist.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utils/containers/hash/hash243_set.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Hash-int map type and operations
 */

typedef struct hash_to_int_map_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  int64_t value;
  UT_hash_handle hh;
} hash_to_int_map_entry_t;

typedef hash_to_int_map_entry_t *hash_int_map_t;

retcode_t hash_int_map_add(hash_int_map_t *const map,
                           flex_trit_t const *const hash, int64_t const value);
bool hash_int_map_contains(hash_int_map_t const *const map,
                           flex_trit_t const *const hash);
void hash_int_map_free(hash_int_map_t *const map);

/*
 * Hash-indexed_hash_set map type and operations
 */

typedef struct hash_to_indexed_hash_set_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243]; /*key*/
  size_t idx;
  hash243_set_t approvers; /*value*/
  UT_hash_handle hh;
} hash_to_indexed_hash_set_entry_t;

typedef hash_to_indexed_hash_set_entry_t *hash_to_indexed_hash_set_map_t;

bool hash_to_indexed_hash_set_map_contains(
    hash_to_indexed_hash_set_map_t const *const map,
    flex_trit_t const *const hash);
retcode_t hash_to_indexed_hash_set_map_add_new_set(
    hash_to_indexed_hash_set_map_t *const map, flex_trit_t const *const hash,
    hash_to_indexed_hash_set_entry_t **const new_set_entry, size_t const index);
void hash_to_indexed_hash_set_map_free(hash_to_indexed_hash_set_map_t *map);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_HASH_CONTAINERS_H__
