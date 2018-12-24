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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash_to_{TYPE}_map_entry_s {
    flex_trit_t hash[FLEX_TRIT_SIZE_243];
    {TYPE} value;
    UT_hash_handle hh;
} hash_to_{TYPE}_map_entry_t;

typedef hash_to_{TYPE}_map_entry_t *hash_to_{TYPE}_map_t;


retcode_t hash_{TYPE}_map_add(hash_to_{TYPE}_map_t *const map,
        flex_trit_t const *const hash,
{TYPE} const value);
bool hash_{TYPE}_map_contains(hash_to_{TYPE}_map_t const *const map,
        flex_trit_t const *const hash);
bool hash_{TYPE}_map_find(hash_to_{TYPE}_map_t const *const map,
        flex_trit_t const *const hash,
hash_to_{TYPE}_map_entry_t const **res);
void hash_{TYPE}_map_free(hash_to_{TYPE}_map_t *const map);


#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_HASH_HASH{TYPE}_MAP_H__
