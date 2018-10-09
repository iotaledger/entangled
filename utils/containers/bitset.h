/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_BITSET_H__
#define __UTILS_CONTAINERS_BITSET_H__

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bitset_s {
  uint64_t* raw_bits;
  uint32_t bitset_integer_index;
  uint32_t bitset_relative_index;
  size_t size;
} bitset_t;

void bitset_reset(bitset_t* const bitset);
bool bitset_is_set(bitset_t* const bitset, size_t pos);
void bitset_set_true(bitset_t* const bitset, size_t pos);

size_t bistset_required_size(size_t num_elements);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_BITSET_H__
