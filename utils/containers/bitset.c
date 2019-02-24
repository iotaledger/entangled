/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "bitset.h"
#include <assert.h>
#include <string.h>

size_t bistset_required_size(size_t num_elements) {
  return (num_elements / (sizeof(int64_t) * 8) + 1);
}

void bitset_reset(bitset_t* const bitset) {
  if (bitset->size == 0) {
    return;
  }
  memset(bitset->raw_bits, 0, bitset->size * sizeof(*(bitset->raw_bits)));
}

bool bitset_is_set(bitset_t* const bitset, size_t pos) {
  bitset->bitset_integer_index = pos / (sizeof(*bitset) * 8);
  bitset->bitset_relative_index = pos % (sizeof(*bitset) * 8);

  return bitset->raw_bits[bitset->bitset_integer_index] &
         (1ULL << bitset->bitset_relative_index);
}

void bitset_set_true(bitset_t* const bitset, size_t pos) {
  bitset->bitset_integer_index = pos / (sizeof(*bitset) * 8);
  bitset->raw_bits[bitset->bitset_integer_index] |= (1ULL << pos);
}
