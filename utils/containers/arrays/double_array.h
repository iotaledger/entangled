/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_ARRAY_DOUBLE_ARRAY_H__
#define __UTILS_CONTAINERS_ARRAY_DOUBLE_ARRAY_H__

#include <inttypes.h>
#include "utarray.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UT_array* double_array_p;

double_array_p double_array_new();

uint32_t double_array_len(double_array_p array);
void double_array_free(double_array_p array);
double double_array_at(double_array_p array, uint32_t index);
void double_array_reserve(double_array_p array, uint32_t len);
void double_array_push(double_array_p array, double value);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_ARRAY_DOUBLE_ARRAY_H__
