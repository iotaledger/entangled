/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_HASH_HASH{SIZE}_SET_H__
#define __UTILS_CONTAINERS_HASH_HASH{SIZE}_SET_H__

#include "uthash.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#define HASH_SET_ITER(set, entry, tmp) HASH_ITER(hh, set, entry, tmp)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash{SIZE}_set_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_{SIZE}];
  UT_hash_handle hh;
} hash{SIZE}_set_entry_t;

typedef hash{SIZE}_set_entry_t *hash{SIZE}_set_t;

typedef retcode_t (*hash{SIZE}_on_container_func)(void *container, flex_trit_t *hash);

uint32_t hash{SIZE}_set_size(hash{SIZE}_set_t const set);
retcode_t hash{SIZE}_set_add(hash{SIZE}_set_t *const set, flex_trit_t const *const hash);
retcode_t hash{SIZE}_set_remove(hash{SIZE}_set_t *const set, flex_trit_t const *const hash);
retcode_t hash{SIZE}_set_remove_entry(hash{SIZE}_set_t *const set, hash{SIZE}_set_entry_t *const entry);
retcode_t hash{SIZE}_set_append(hash{SIZE}_set_t const *const set1, hash{SIZE}_set_t *const set2);
bool hash{SIZE}_set_contains(hash{SIZE}_set_t const set, flex_trit_t const *const hash);
bool hash{SIZE}_set_find(hash{SIZE}_set_t const set, flex_trit_t const *const hash, hash{SIZE}_set_entry_t **entry);
void hash{SIZE}_set_free(hash{SIZE}_set_t *const set);
retcode_t hash{SIZE}_set_for_each(hash{SIZE}_set_t const set, hash{SIZE}_on_container_func func,
                                   void *const container);
retcode_t hash{SIZE}_set_random_hash(hash{SIZE}_set_t const *const set, flex_trit_t *const hash);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_HASH_HASH{SIZE}_SET_H__
