/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_HASH_HASH_ARRAY_H__
#define __UTILS_CONTAINERS_HASH_HASH_ARRAY_H__

#include "utarray.h"

#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UT_array* hash_array_p;
typedef hash_array_p hash27_array_p;
typedef hash_array_p hash81_array_p;
typedef hash_array_p hash243_array_p;
typedef hash_array_p hash6561_array_p;
typedef hash_array_p hash8019_array_p;

hash27_array_p hash27_array_new();
hash81_array_p hash81_array_new();
hash243_array_p hash243_array_new();
hash6561_array_p hash6561_array_new();
hash8019_array_p hash8019_array_new();

void hash_array_push(hash_array_p array, flex_trit_t const* const trits);
uint32_t hash_array_len(hash_array_p array);
void hash_array_free(hash_array_p array);
flex_trit_t* hash_array_at(hash_array_p array, uint32_t index);
void hash_array_reserve(hash_array_p array, uint32_t len);

#define HASH_ARRAY_FOREACH(array, elt) \
  for (elt = (flex_trit_t*)utarray_front(array); elt != NULL; elt = (flex_trit_t*)utarray_next(array, elt))

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_HASH_HASH_ARRAY_H__
