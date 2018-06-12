/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_TRINARY_TRIT_ARRAY_H_
#define __COMMON_TRINARY_TRIT_ARRAY_H_

#if !defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE) && \
    !defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
#define TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE
#endif

#include "trit_byte.h"
#include "trits.h"

typedef int8_t flex_trit_t;

static inline size_t flex_trits_num_for_trits(size_t num_trits) {
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  return num_trits;
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  return (num_trits + 3) >> 2U;
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
  return min_bytes(num_trits);
#endif
}

static inline trit_t flex_trit_array_at(const flex_trit_t *trit_array,
                                        size_t index) {
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  // Straight forward 1 trit per byte
  return trit_array[index];
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  // Find out the position of the trit in the byte
  uint8_t mshift = (index % 4U) << 1U;
  // Calculate the shift to sign extend the result
  uint8_t tshift = 6U - mshift;
  // Find out the index of the byte in the array
  index = index >> 2U;
  // Get the trit and sign extend it
  return (flex_trit_t)(trit_array[index] << tshift) >> 6U;
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
  trit_t trits[5];
  // Find out the index of the trit in the byte
  uint8_t tindex = index % 5U;
  // Find out the index of the byte in the array
  index = index / 5U;
  bytes_to_trits(((byte_t *)trit_array + index), 1, trits, 5);
  return trits[tindex];
#endif
}

static inline void flex_trit_array_set_at(flex_trit_t *trit_array, size_t index,
                                          trit_t trit) {
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  // Straight forward 1 trit per byte
  trit_array[index] = trit;
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  // Calculate the final position of the trit in the byte
  uint8_t shift = (index % 4U) << 1U;
  // Position the trit at its place in the byte
  trit = (trit & 0x03) << shift;
  // Create a mask to reset the target trit
  uint8_t mask = ~(0x03 << shift);
  // Find out the index of the byte in the array
  index = index >> 2U;
  // bitblit the trit in place
  trit_array[index] = (trit_array[index] & mask) | trit;
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
  byte_t buffer = 0;
  trit_t trits[5];
  // Find out the index of the trit in the byte
  uint8_t tindex = index % 5U;
  // Find out the index of the byte in the array
  index = index / 5U;
  bytes_to_trits(((byte_t *)trit_array + index), 1, trits, 5);
  trits[tindex] = trit;
  trit_array[index] = trits_to_byte(trits, buffer, 5);
#endif
}

size_t flex_trit_array_slice(const flex_trit_t *trit_array,
                             flex_trit_t *to_trit_array, size_t start,
                             size_t num_trits);
size_t flex_trit_array_to_int8(const flex_trit_t *trit_array, trit_t *trits,
                               size_t num_trits);

typedef struct _trit_array *trit_array_p;
typedef struct _trit_array {
  flex_trit_t *trits;
  size_t num_trits;
  size_t num_bytes;
  uint8_t dynamic;
} trit_array_t;

size_t trit_array_bytes_for_trits(size_t num_trits);

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
static inline trit_t trit_array_at(trit_array_p trit_array, size_t index) {
  return flex_trit_array_at(trit_array->trits, index);
}

static inline void trit_array_set_at(trit_array_p trit_array, size_t index,
                                     trit_t trit) {
  flex_trit_array_set_at(trit_array->trits, index, trit);
}

void trit_array_set_trits(trit_array_p trit_array, flex_trit_t *trits,
                          size_t num_trits);
trit_array_p trit_array_slice(trit_array_p trit_array,
                              trit_array_p to_trit_array, size_t start,
                              size_t num_trits);
trit_t *trit_array_to_int8(trit_array_p trit_array, trit_t *trits);

#if !defined(NO_DYNAMIC_ALLOCATION)
/***********************************************************************************************************
 * Constructor
 ***********************************************************************************************************/
// num_trits is number of trits
trit_array_p trit_array_new(size_t num_trits);

/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
void trit_array_free(trit_array_p trit_array);

#endif  // NO_DYNAMIC_ALLOCATION

#define TRIT_ARRAY_DECLARE(NAME, NUM_TRITS)                          \
  size_t NAME##_num_bytes = trit_array_bytes_for_trits(NUM_TRITS);   \
  flex_trit_t *NAME##_trits[NAME##_num_bytes];                       \
  struct _trit_array NAME = {(flex_trit_t *)NAME##_trits, NUM_TRITS, \
                             NAME##_num_bytes};

#define TRIT_ARRAY_ASSIGN(NAME, NUM_TRITS, TRITS)                    \
  size_t NAME##_num_bytes = trit_array_bytes_for_trits(NUM_TRITS);   \
  flex_trit_t NAME##_trits[] = {TRITS};                              \
  struct _trit_array NAME = {(flex_trit_t *)NAME##_trits, NUM_TRITS, \
                             NAME##_num_bytes};

#endif
#ifdef __cplusplus
}
#endif
