/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "common/defs.h"
#include "common/trinary/trit_long.h"

static trit_t const encoded_zero[] = {1, 0, 0, -1};
static double const pow27LUT[] = {1,
                                  27,
                                  729,
                                  19683,
                                  531441,
                                  14348907,
                                  387420489,
                                  10460353203,
                                  282429536481,
                                  7625597484987,
                                  205891132094649,
                                  5559060566555523,
                                  150094635296999136,
                                  4052555153018976256};

int64_t trits_to_long(trit_t const *const trits, size_t const num_trits) {
  if (num_trits == 0) {
    return 0;
  }
  int64_t accum = 0;
  size_t end = num_trits;
  while (end-- > 0) {
    accum = accum * RADIX + trits[end];
  }
  return accum;
}

size_t min_trits(int64_t const value) {
  size_t num = 1;
  uint64_t vp = 1;
  uint64_t v_abs;
  // Edge case where value == INT64_MIN. In this case,
  // llabs cannot return a value greater than INT64_MAX
  // so we "force" the (unsigned) value explicitly
  if (value == INT64_MIN) {
    v_abs = INT64_MAX + 1ULL;
  } else {
    v_abs = llabs(value);
  }
  while (v_abs > vp) {
    vp = vp * RADIX + 1;
    num++;
  }
  return num;
}

size_t long_to_trits(int64_t const value, trit_t *const trits) {
  trit_t trit;
  size_t i, size;
  uint64_t v_abs;
  char negative;

  negative = value < 0;
  size = min_trits(value);
  // Edge case where value == INT64_MIN. In this case,
  // llabs cannot return a value greater than INT64_MAX
  // so we "force" the (unsigned) value explicitly
  if (value == INT64_MIN) {
    v_abs = INT64_MAX + 1ULL;
  } else {
    v_abs = llabs(value);
  }
  memset(trits, 0, RADIX);
  for (i = 0; i < size; i++) {
    if (v_abs == 0) {
      break;
    }
    trit = (v_abs + 1) % (RADIX)-1;
    if (negative) {
      trit = -trit;
    }
    trits[i] = trit;
    v_abs++;
    v_abs /= RADIX;
  }
  return size;
}

size_t nearest_greater_multiple_of_three(size_t const value) {
  size_t rem = value % RADIX;
  if (rem == 0) {
    return value;
  }
  return value + RADIX - rem;
}

size_t encoded_length(int64_t const value) {
  if (value == 0) {
    return sizeof(encoded_zero) / sizeof(trit_t);
  }
  size_t length = nearest_greater_multiple_of_three(min_trits(llabs(value)));
  // trits length + encoding length
  return length + min_trits((1 << (length / RADIX)) - 1);
}

int encode_long(int64_t const value, trit_t *const trits,
                size_t const num_trits) {
  if (num_trits < encoded_length(value)) {
    return -1;
  }
  if (value == 0) {
    memcpy(trits, encoded_zero, encoded_length(0));
    return 0;
  }

  size_t encoding = 0;
  size_t index = 0;
  size_t length = nearest_greater_multiple_of_three(min_trits(llabs(value)));

  long_to_trits(value, trits);
  for (size_t i = 0; i < length - RADIX; i += RADIX, index += 1) {
    if (trits_to_long(&trits[i], RADIX) >= 0) {
      encoding |= 1 << index;
      for (size_t j = 0; j < RADIX; j++) trits[i + j] = -trits[i + j];
    }
  }
  if (trits_to_long(&trits[length - RADIX], RADIX) <= 0) {
    encoding |= 1 << index;
    for (size_t i = 1; i < RADIX + 1; i++)
      trits[length - i] = -trits[length - i];
  }
  long_to_trits(encoding, &trits[length]);
  return 0;
}

int64_t decode_long(trit_t const *const trits, size_t const num_trits,
                    size_t *const size) {
  if (memcmp(trits, encoded_zero, encoded_length(0)) == 0) {
    *size = encoded_length(0);
    return 0;
  }
  int64_t value = 0;
  size_t encoding_start = 0;
  while (encoding_start < num_trits &&
         trits_to_long(&trits[encoding_start], RADIX) <= 0)
    encoding_start += RADIX;
  if (encoding_start >= num_trits) {
    return -1;
  }
  encoding_start += RADIX;
  size_t encoding_length = min_trits((1 << (encoding_start / RADIX)) - 1);
  size_t encoding = trits_to_long(&trits[encoding_start], encoding_length);
  // Bound checking for the lookup table
  if (encoding_start / RADIX > 13) {
    return -1;
  }
  for (size_t i = 0; i < encoding_start / RADIX; i += 1) {
    int64_t tryte_value = trits_to_long(&trits[i * RADIX], RADIX);
    if ((encoding >> i) & 1) {
      tryte_value = -tryte_value;
    }
    value += pow27LUT[i] * tryte_value;
  }
  *size = encoding_start + encoding_length;
  return value;
}
