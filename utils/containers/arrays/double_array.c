/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/arrays/double_array.h"

static UT_icd double_array_icd = {sizeof(double), NULL, NULL, NULL};

double_array_p double_array_new() {
  double_array_p array = NULL;
  utarray_new(array, &double_array_icd);
  return array;
}

void double_array_push(double_array_p array, double value) {
  utarray_push_back(array, &value);
}

double double_array_at(double_array_p array, uint32_t index) {
  return *(double *)utarray_eltptr(array, index);
}

void double_array_reserve(double_array_p array, uint32_t len) {
  utarray_reserve(array, len);
}

uint32_t double_array_len(double_array_p array) { return utarray_len(array); }

void double_array_free(double_array_p array) { utarray_free(array); }
