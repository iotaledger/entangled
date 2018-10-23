/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_SETS_HASH$SIZE_SET_H__
#define __UTILS_CONTAINERS_SETS_HASH$SIZE_SET_H__

#include "uthash.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash$SIZE_set_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_$SIZE];
  UT_hash_handle hh;
} hash$SIZE_set_entry_t;

typedef hash$SIZE_set_entry_t *hash$SIZE_set_t;

typedef retcode_t (*hash$SIZE_on_container_func)(void *container,
                                                 flex_trit_t *hash);

uint32_t hash$SIZE_set_size(hash$SIZE_set_t const *const set);
retcode_t hash$SIZE_set_add(hash$SIZE_set_t *const set,
                            flex_trit_t const *const hash);
retcode_t hash$SIZE_set_append(hash$SIZE_set_t const *const set1,
                               hash$SIZE_set_t *const set2);
bool hash$SIZE_set_contains(hash$SIZE_set_t const *const set,
                            flex_trit_t const *const hash);
void hash$SIZE_set_free(hash$SIZE_set_t *const set);
retcode_t hash$SIZE_set_for_each(hash$SIZE_set_t const *const set,
                                 hash$SIZE_on_container_func func,
                                 void *const container);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_SETS_HASH$SIZE_SET_H__
