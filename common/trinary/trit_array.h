/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled/
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_TRINARY_TRIT_ARRAY_H_
#define __COMMON_TRINARY_TRIT_ARRAY_H_

#if !defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE) && \
    !defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE) && \
    !defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
#define TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE
#endif

#include "trit_byte.h"
#include "trits.h"

typedef int8_t flex_trit_t;

/// Returns the number of bytes needed to store a given number of trits in the
/// current memory model.
/// @param[in] num_trits - number of trits to store
/// @return size_t - the number of bytes need
static inline size_t flex_trits_num_for_trits(size_t num_trits) {
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  return num_trits;
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  return (num_trits + 3) >> 2U;
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
  return min_bytes(num_trits);
#endif
}

/// Returns the trit at a given index in an array of trits
/// @param[in] trit_array - an array of trits
/// @param[in] index - the index of the trit to access
/// @return trit_t - the trit at the given index
static inline trit_t flex_trit_array_at(const flex_trit_t *trit_array,
                                        size_t len, size_t index) {
  // Bounds checking
  if (index >= len) {
    return 0;
  }
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  // Straight forward 1 trit per byte
  return trit_array[index];
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  // Find out the position of the trit in the byte
  uint8_t mshift = (index & 3) << 1U;
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

/// Set the trit at a given index in an array of trits to the given value
/// @param[in] trit_array - an array of trits
/// @param[in] index - the index of the trit to access
/// @param[in] trit - the trit value to set
static inline uint8_t flex_trit_array_set_at(flex_trit_t *trit_array, size_t len,
                                             size_t index, trit_t trit) {
  // Bounds checking
  if (index >= len) {
    return 0;
  }
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  // Straight forward 1 trit per byte
  trit_array[index] = trit;
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  // Calculate the final position of the trit in the byte
  uint8_t shift = (index & 3) << 1U;
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
  return 1;
}

/// Returns a portion of length num_trits of an array into a new array from
/// start. The original array will not be modified.
/// @param[in] to_trit_array - the array containing the selected portion
/// @param[in] to_len - the number of trits the target array can store
/// @param[in] trit_array - the original array
/// @param[in] len - the number of trits the original array stores
/// @param[in] start - the start index in the original array
/// @param[in] num_trits - the number of trits to copy over
/// @return size_t - the number of trits copied over
size_t flex_trit_array_slice(flex_trit_t *to_trit_array, size_t to_len,
                             const flex_trit_t *trit_array, size_t len,
                             size_t start, size_t num_trits);

/// Inserts the contents of an array into another array starting at a given
/// index.
/// @param[in] trit_array - the array to insert into
/// @param[in] from_trit_array - the array containing the trits to copy over
/// @param[in] start - the start index in the target array
/// @param[in] num_trits - the number of trits to copy over
/// @return size_t - the number of trits copied over
size_t flex_trit_array_insert(flex_trit_t *to_trit_array, size_t to_len,
                              const flex_trit_t *trit_array, size_t len,
                              size_t start, size_t num_trits);

/// Returns an array of trits regardless of the current memory storage scheme
/// @param[in] trits - an array of individual trits
/// @param[in] trit_array - the array of packed trits
/// @param[in] num_trits - the number of trits to extract
/// @return size_t - the number of trits extracted
size_t flex_trit_array_to_int8(trit_t *trits, size_t to_len,
                               const flex_trit_t *trit_array, size_t len,
                               size_t num_trits);

/// Returns an array of trits packed in the current memory storage scheme
/// @param[in] trit_array - the array of packed trits
/// @param[in] trits - an array of individual trits
/// @param[in] num_trits - the number of trits to pack
/// @return size_t - the number of trits packed
size_t int8_to_flex_trit_array(flex_trit_t *to_trit_array, size_t to_len,
                               const trit_t *trits, size_t len,
                               size_t num_trits);

/***********************************************************************************************************
 * Trits array
 ***********************************************************************************************************/
/// A trit array represents an array of trits packed according to the current
/// memroy model.
typedef struct _trit_array *trit_array_p;
typedef struct _trit_array {
  flex_trit_t *trits;
  size_t num_trits;
  size_t num_bytes;
  uint8_t dynamic;
} trit_array_t;

/// Returns the number of bytes needed to pack a given number of trits in the
/// current memory model.
/// @param[in] num_trits - the number of trits to pack
/// @return size_t - the number of bytes needed
size_t trit_array_bytes_for_trits(size_t num_trits);

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
/// Returns the trit at a given index in an array of trits
/// @param[in] trit_array - a pointer to a tri_array_t
/// @param[in] index - the index of the trit to access
/// @return trit_t - the trit at the given index
static inline trit_t trit_array_at(trit_array_p trit_array, size_t index) {
  return flex_trit_array_at(trit_array->trits, trit_array->num_trits, index);
}

/// Set the trit at a given index in an array of trits to the given value
/// @param[in] trit_array - a pointer to a tri_array_t
/// @param[in] index - the index of the trit to access
/// @param[in] trit - the trit value to set
static inline uint8_t trit_array_set_at(trit_array_p trit_array, size_t index,
                                        trit_t trit) {
  return flex_trit_array_set_at(trit_array->trits, trit_array->num_trits, index, trit);
}

void trit_array_set_trits(trit_array_p trit_array, flex_trit_t *trits,
                          size_t num_trits);

/// Returns a new trit_array containing a portion of length num_trits of the
/// trits of another trit_array, from index start.
/// @param[in] trit_array - the original trit_array
/// @param[in] to_trit_array - the new trit_array
/// @param[in] start - the start index in the original array
/// @param[in] num_trits - the number of trits to copy over
/// @return trit_array_p - the new trit_array
trit_array_p trit_array_slice(trit_array_p trit_array,
                              trit_array_p to_trit_array, size_t start,
                              size_t num_trits);

/// Returns an array of trits regardless of the current memory storage scheme
/// @param[in] trit_array - the array of packed trits
/// @param[in] trits - an array of individual trits
/// @return trit_t - the array of individual trits
trit_t *trit_array_to_int8(trit_array_p trit_array, trit_t *trits, size_t len);

#if !defined(NO_DYNAMIC_ALLOCATION)
/***********************************************************************************************************
 * Constructor
 ***********************************************************************************************************/
/// Returns a new trit_array that can accomodate num_trits trits
/// @param[in] num_trits - the number of trits to store
/// @return trit_array_p - the new trit_array
trit_array_p trit_array_new(size_t num_trits);

/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
void trit_array_free(trit_array_p trit_array);

#endif  // NO_DYNAMIC_ALLOCATION

/// Macro to declare a new trit_array.
/// @param[in] NAME - the name of the variable to declare
/// @param[in] NUM_TRITS - the number of trits the trit_array can store
#define TRIT_ARRAY_DECLARE(NAME, NUM_TRITS)                          \
  size_t NAME##_num_bytes = trit_array_bytes_for_trits(NUM_TRITS);   \
  flex_trit_t NAME##_trits[NAME##_num_bytes];                       \
  struct _trit_array NAME = {(flex_trit_t *)&NAME##_trits, NUM_TRITS, \
                             NAME##_num_bytes};

/// Macro to declare a new trit_array and initialize it with an array of trits
/// stored according to the current memory model.
/// @param[in] NAME - the name of the varialbe to declare
/// @param[in] NUM_TRITS - the number of trits the trit_array can store
/// @param[in] TRITS - the initial trits packed in to the current memory model
#define TRIT_ARRAY_MAKE(NAME, NUM_TRITS, TRITS)                    \
  size_t NAME##_num_bytes = trit_array_bytes_for_trits(NUM_TRITS);   \
  flex_trit_t NAME##_trits[] = {TRITS};                              \
  struct _trit_array NAME = {(flex_trit_t *)NAME##_trits, NUM_TRITS, \
                             NAME##_num_bytes};

#endif
#ifdef __cplusplus
}
#endif
