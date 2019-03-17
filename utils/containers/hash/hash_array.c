/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/hash/hash_array.h"

static UT_icd ut_hash27_icd = {FLEX_TRIT_SIZE_27, NULL, NULL, NULL};
static UT_icd ut_hash81_icd = {FLEX_TRIT_SIZE_81, NULL, NULL, NULL};
static UT_icd ut_hash243_icd = {FLEX_TRIT_SIZE_243, NULL, NULL, NULL};
static UT_icd ut_hash6561_icd = {FLEX_TRIT_SIZE_6561, NULL, NULL, NULL};
static UT_icd ut_hash8019_icd = {FLEX_TRIT_SIZE_8019, NULL, NULL, NULL};

hash27_array_p hash27_array_new() {
  hash27_array_p array = NULL;
  utarray_new(array, &ut_hash27_icd);
  return array;
}

hash81_array_p hash81_array_new() {
  hash81_array_p array = NULL;
  utarray_new(array, &ut_hash81_icd);
  return array;
}

hash243_array_p hash243_array_new() {
  hash243_array_p array = NULL;
  utarray_new(array, &ut_hash243_icd);
  return array;
}

hash6561_array_p hash6561_array_new() {
  hash6561_array_p array = NULL;
  utarray_new(array, &ut_hash6561_icd);
  return array;
}

hash8019_array_p hash8019_array_new() {
  hash8019_array_p array = NULL;
  utarray_new(array, &ut_hash8019_icd);
  return array;
}

void hash_array_push(hash_array_p array, flex_trit_t const* const trits) { utarray_push_back(array, trits); }

flex_trit_t* hash_array_at(hash_array_p array, uint32_t index) { return (flex_trit_t*)utarray_eltptr(array, index); }

void hash_array_reserve(hash_array_p array, uint32_t len) { utarray_reserve(array, len); }

uint32_t hash_array_len(hash_array_p array) { return utarray_len(array); }

void hash_array_free(hash_array_p array) { utarray_free(array); }
