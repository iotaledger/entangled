/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_HASH_HASH{TYPE}_MAP_H__
#define __UTILS_CONTAINERS_HASH_HASH{TYPE}_MAP_H__

#include "uthash.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utils/containers/hash/hash243_set.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash_to_{TYPE}_map_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  {TYPE} value;
  UT_hash_handle hh;
} hash_to_{TYPE}_map_entry_t;

typedef hash_to_{TYPE}_map_entry_t *hash_to_{TYPE}_map_t;

size_t hash_to_{TYPE}_map_size(hash_to_{TYPE}_map_t map);

retcode_t hash_to_{TYPE}_map_add(hash_to_{TYPE}_map_t *const map, flex_trit_t const *const hash,
                                  {TYPE} const value);
bool hash_to_{TYPE}_map_contains(hash_to_{TYPE}_map_t const map, flex_trit_t const *const hash);
bool hash_to_{TYPE}_map_remove(hash_to_{TYPE}_map_t *const map, flex_trit_t const *const hash);
bool hash_to_{TYPE}_map_find(hash_to_{TYPE}_map_t const map, flex_trit_t const *const hash,
                              hash_to_{TYPE}_map_entry_t **res);
void hash_to_{TYPE}_map_free(hash_to_{TYPE}_map_t *const map);
void hash_to_{TYPE}_map_keys(hash_to_{TYPE}_map_t *const map, hash243_set_t *const keys);

retcode_t hash_to_{TYPE}_map_copy(hash_to_{TYPE}_map_t const *const src, hash_to_{TYPE}_map_t *const dst);

bool hash_to_{TYPE}_map_equal(hash_to_{TYPE}_map_t const lhs, hash_to_{TYPE}_map_t const rhs);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_HASH_HASH{TYPE}_MAP_H__
