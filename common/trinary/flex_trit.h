/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_TRINARY_FLEX_TRIT_H_
#define __COMMON_TRINARY_FLEX_TRIT_H_

#if !defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE) && !defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE) && \
    !defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE) && !defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
#define FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE
#endif

#include <stdbool.h>
#include <string.h>

#include "common/defs.h"
#include "common/trinary/trit_byte.h"
#include "common/trinary/trit_tryte.h"

typedef int8_t flex_trit_t;

#if defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
#define FLEX_TRIT_NULL_VALUE '9'
#else
#define FLEX_TRIT_NULL_VALUE 0
#endif

#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
#define FLEX_TRIT_SIZE_27 27
#define FLEX_TRIT_SIZE_81 81
#define FLEX_TRIT_SIZE_243 243
#define FLEX_TRIT_SIZE_6561 6561
#define FLEX_TRIT_SIZE_8019 8019
#define NUM_TRITS_PER_FLEX_TRIT 1
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
#define FLEX_TRIT_SIZE_27 9
#define FLEX_TRIT_SIZE_81 27
#define FLEX_TRIT_SIZE_243 81
#define FLEX_TRIT_SIZE_6561 2187
#define FLEX_TRIT_SIZE_8019 2673
#define NUM_TRITS_PER_FLEX_TRIT 3
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
#define FLEX_TRIT_SIZE_27 7
#define FLEX_TRIT_SIZE_81 21
#define FLEX_TRIT_SIZE_243 61
#define FLEX_TRIT_SIZE_6561 1641
#define FLEX_TRIT_SIZE_8019 2005
#define NUM_TRITS_PER_FLEX_TRIT 4
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
#define FLEX_TRIT_SIZE_27 6
#define FLEX_TRIT_SIZE_81 17
#define FLEX_TRIT_SIZE_243 49
#define FLEX_TRIT_SIZE_6561 1313
#define FLEX_TRIT_SIZE_8019 1604
#define NUM_TRITS_PER_FLEX_TRIT 5
#endif

/// Returns the number of bytes needed to store a given number of trits in the
/// current memory model.
/// @param[in] num_trits - number of trits to store
/// @return size_t - the number of flex_trits (bytes) needed
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
#define NUM_FLEX_TRITS_FOR_TRITS(num_trits) num_trits
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
#define NUM_FLEX_TRITS_FOR_TRITS(num_trits) ((num_trits + NUMBER_OF_TRITS_IN_A_TRYTE - 1) / NUMBER_OF_TRITS_IN_A_TRYTE)
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
#define NUM_FLEX_TRITS_FOR_TRITS(num_trits) ((num_trits + 3) >> 2U)
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
#define NUM_FLEX_TRITS_FOR_TRITS(num_trits) ((num_trits + NUMBER_OF_TRITS_IN_A_BYTE - 1) / NUMBER_OF_TRITS_IN_A_BYTE)
#endif

/// Returns the trit at a given index in an array of flex_trits
/// @param[in] flex_trits - an array of flex_trits
/// @param[in] len - the number of trits encoded in the flex_trit array
/// @param[in] index - the index of the trit to access
/// @return trit_t - the trit at the given index
static inline trit_t flex_trits_at(flex_trit_t const *const flex_trits, size_t const len, size_t index) {
  // Bounds checking
  if (index >= len) {
    return 0;
  }
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  // Straight forward 1 trit per byte
  return flex_trits[index];
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  return get_trit_at((tryte_t *)flex_trits, NUM_FLEX_TRITS_FOR_TRITS(len), index);
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  // Find out the position of the trit in the byte
  uint8_t mshift = (index & 3) << 1U;
  // Calculate the shift to sign extend the result
  uint8_t tshift = 6U - mshift;
  // Find out the index of the byte in the array
  index = index >> 2U;
  // Get the trit and sign extend it
  return (flex_trit_t)(flex_trits[index] << tshift) >> 6U;
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  trit_t trits[5];
  // Find out the index of the trit in the byte
  uint8_t tindex = index % 5U;
  // Find out the index of the byte in the array
  index = index / 5U;
  byte_to_trits(*(flex_trits + index), trits, 5);
  return trits[tindex];
#endif
}

/// Set the trit at a given index in an array of trits to the given value
/// @param[in] flex_trits - an array of flex_trits
/// @param[in] len - the number of trits encoded in the flex_trit array
/// @param[in] index - the index of the trit to access
/// @param[in] trit - the trit value to set
static inline uint8_t flex_trits_set_at(flex_trit_t *const flex_trits, size_t const len, size_t index, trit_t trit) {
  // Bounds checking
  if (index >= len) {
    return 0;
  }
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  // Straight forward 1 trit per byte
  flex_trits[index] = trit;
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  set_trit_at(flex_trits, NUM_FLEX_TRITS_FOR_TRITS(len), index, trit);
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  // Calculate the final position of the trit in the byte
  uint8_t shift = (index & 3) << 1U;
  // Position the trit at its place in the byte
  trit = (trit & 0x03) << shift;
  // Create a mask to reset the target trit
  uint8_t mask = ~(0x03 << shift);
  // Find out the index of the byte in the array
  index = index >> 2U;
  // bitblit the trit in place
  flex_trits[index] = (flex_trits[index] & mask) | trit;
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  trit_t trits[5];
  // Find out the index of the trit in the byte
  uint8_t tindex = index % 5U;
  // Find out the index of the byte in the array
  index = index / 5U;
  byte_to_trits(*(flex_trits + index), trits, 5);
  trits[tindex] = trit;
  flex_trits[index] = trits_to_byte(trits, 5);
#endif
  return 1;
}

// return true if all flex_trit are NULL, else false.
static inline bool flex_trits_are_null(flex_trit_t const *const flex_trits, size_t const len) {
  for (size_t i = 0; i < len; i++) {
    if (flex_trits[i] != FLEX_TRIT_NULL_VALUE) {
      return false;
    }
  }
  return true;
}

/// Returns a portion of length num_trits of an array into a new array from
/// start. The original array will not be modified.
/// @param[in] to_flex_trits - the array that will contain the slice
/// @param[in] to_len - the number of trits encoded in the to_flex_trits array
/// @param[in] flex_trits - the original array
/// @param[in] len - the number of trits the flex_trits array stores
/// @param[in] start - the start index in the original array
/// @param[in] num_trits - the number of trits to extract
/// @return size_t - the number of trits extracted
size_t flex_trits_slice(flex_trit_t *const to_flex_trits, size_t const to_len, flex_trit_t const *const flex_trits,
                        size_t const len, size_t const start, size_t const num_trits);

/// Inserts the contents of an array into another array starting at a given
/// index.
/// @param[in] to_flex_trits - the array to insert into
/// @param[in] to_len - the number of trits encoded in the to_flex_trits array
/// @param[in] flex_trits - the array containing the trits to copy over
/// @param[in] len - the number of trits the flex_trits array stores
/// @param[in] start - the start index in the destination array
/// @param[in] num_trits - the number of trits to copy over
/// @return size_t - the number of trits copied over
size_t flex_trits_insert(flex_trit_t *const to_flex_trits, size_t const to_len, flex_trit_t const *const flex_trits,
                         size_t const len, size_t const start, size_t const num_trits);

/// Inserts the contents of an array into another array starting at a given
/// index.
/// @param[in] dst_trits - the array to insert into
/// @param[in] dst_len - the number of trits encoded in the to_flex_trits array
/// @param[in] src_trits - the array containing the trits to copy over
/// @param[in] src_len - the number of trits the flex_trits array stores
/// @param[in] src_start_pos - the start index on the source array
/// @param[in] dst_start_pos - the start index on the destination array
/// @param[in] num_trits - the number of trits to copy over
/// @return size_t - the number of trits copied over
size_t flex_trits_insert_from_pos(flex_trit_t *const dst_trits, size_t const dst_len,
                                  flex_trit_t const *const src_trits, size_t const src_len, size_t const src_start_pos,
                                  size_t const dst_start_pos, size_t const num_trits);

/// Returns an array of trits regardless of the current memory storage
/// scheme
/// @param[in] trits - an array of individual trits
/// @param[in] to_len - the number of trits the trits array contains
/// @param[in] flex_trits - the array of packed trits
/// @param[in] len - the number of trits the flex_trits array stores
/// @param[in] num_trits - the number of trits to extract
/// @return size_t - the number of trits encoded
size_t flex_trits_to_trits(trit_t *const trits, size_t const to_len, flex_trit_t const *const flex_trits,
                           size_t const len, size_t const num_trits);

/// Returns an array of trits packed in the current memory storage scheme
/// @param[in] to_flex_trits - an array of flex_trits
/// @param[in] to_len - the number of trits encoded in the to_flex_trits array
/// @param[in] trits - an array of individual trits
/// @param[in] len - the number of trits the trits array contains
/// @param[in] num_trits - the number of trits to pack
/// @return size_t - the number of trits decoded
size_t flex_trits_from_trits(flex_trit_t *const to_flex_trits, size_t const to_len, trit_t const *const trits,
                             size_t const len, size_t const num_trits);

/// Returns an array of trytes.
/// @param[in] trytes - an array to store trytes
/// @param[in] to_len - the number of trytes the trytes array contains
/// @param[in] flex_trits - the array of packed trits
/// @param[in] len - the number of trits the flex_trits array contains
/// @param[in] num_trits - the number of trits to pack
/// @return size_t - the number of trits encoded
size_t flex_trits_to_trytes(tryte_t *trytes, size_t to_len, const flex_trit_t *flex_trits, size_t len,
                            size_t num_trits);

/// Returns an array of flex_trits.
/// @param[in] to_flex_trits - the array of packed trits
/// @param[in] to_len - the number of trits in the to_flex_trits array
/// @param[in] trytes - an array of trytes
/// @param[in] len - the number of trytes in the trytes array
/// @param[in] num_trytes - the size of trytes to unpack
/// @return size_t - the number of trytes decoded
size_t flex_trits_from_trytes(flex_trit_t *to_flex_trits, size_t to_len, const tryte_t *trytes, size_t len,
                              size_t num_trytes);

/// Returns an array of bytes.
/// @param[in] bytes - an array to store bytes
/// @param[in] to_len - the number of trits the bytes array contains
/// @param[in] flex_trits - the array of packed trits
/// @param[in] len - the number of trits the flex_trits array contains
/// @param[in] num_trits - the number of trits to pack
/// @return size_t - the number of trits encoded
size_t flex_trits_to_bytes(byte_t *bytes, size_t to_len, const flex_trit_t *flex_trits, size_t len, size_t num_trits);

/// Returns an array of flex_trits.
/// @param[in] to_flex_trits - the array of packed trits
/// @param[in] to_len - the number of trits in the to_flex_trits array
/// @param[in] bytes - an array of bytes
/// @param[in] len - the number of trits in the bytes array
/// @param[in] num_trits - the number of trits to unpack
/// @return size_t - the number of trits decoded
size_t flex_trits_from_bytes(flex_trit_t *to_flex_trits, size_t to_len, const byte_t *bytes, size_t len,
                             size_t num_trits);

/**
 * @brief Print flex trits through stdout
 *
 * @param trits A pointer to flex trits
 * @param trits_len Number of trit
 */
void flex_trit_print(flex_trit_t const *trits, size_t trits_len);

#endif
#ifdef __cplusplus
}
#endif  // __COMMON_TRINARY_FLEX_TRIT_H_
