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

#if !defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE) &&  \
    !defined(TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE) && \
    !defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE) && \
    !defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
#define TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE
#endif

#include <string.h>

#include "common/trinary/trit_byte.h"
#include "common/trinary/trit_tryte.h"
#include "common/trinary/trits.h"

typedef int8_t flex_trit_t;

#if defined(TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE)
#define FLEX_TRIT_NULL_VALUE '9'
#else
#define FLEX_TRIT_NULL_VALUE 0
#endif

/// Returns the number of bytes needed to store a given number of trits in the
/// current memory model.
/// @param[in] num_trits - number of trits to store
/// @return size_t - the number of bytes need
static inline size_t flex_trits_num_for_trits(size_t const num_trits) {
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  return num_trits;
#elif defined(TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE)
  return num_trytes_for_trits(num_trits);
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
static inline trit_t flex_trit_array_at(flex_trit_t const *const trit_array,
                                        size_t const len, size_t index) {
  // Bounds checking
  if (index >= len) {
    return 0;
  }
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  // Straight forward 1 trit per byte
  return trit_array[index];
#elif defined(TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE)
  return get_trit_at((tryte_t *)trit_array, flex_trits_num_for_trits(len),
                     index);
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
  byte_to_trits(*(trit_array + index), trits, 4);
  return trits[tindex];
#endif
}

/// Set the trit at a given index in an array of trits to the given value
/// @param[in] trit_array - an array of trits
/// @param[in] index - the index of the trit to access
/// @param[in] trit - the trit value to set
static inline uint8_t flex_trit_array_set_at(flex_trit_t *const trit_array,
                                             size_t const len, size_t index,
                                             trit_t trit) {
  // Bounds checking
  if (index >= len) {
    return 0;
  }
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  // Straight forward 1 trit per byte
  trit_array[index] = trit;
#elif defined(TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE)
  set_trit_at(trit_array, flex_trits_num_for_trits(len), index, trit);
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
  trit_t trits[5];
  // Find out the index of the trit in the byte
  uint8_t tindex = index % 5U;
  // Find out the index of the byte in the array
  index = index / 5U;
  byte_to_trits(*(trit_array + index), trits, 4);
  trits[tindex] = trit;
  trit_array[index] = trits_to_byte(trits, 0, 4);
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
size_t flex_trit_array_slice(flex_trit_t *const to_trit_array,
                             size_t const to_len,
                             flex_trit_t const *const trit_array,
                             size_t const len, size_t const start,
                             size_t const num_trits);

/// Inserts the contents of an array into another array starting at a given
/// index.
/// @param[in] trit_array - the array to insert into
/// @param[in] from_trit_array - the array containing the trits to copy over
/// @param[in] start - the start index in the target array
/// @param[in] num_trits - the number of trits to copy over
/// @return size_t - the number of trits copied over
size_t flex_trit_array_insert(flex_trit_t *const to_trit_array,
                              size_t const to_len,
                              flex_trit_t const *const trit_array,
                              size_t const len, size_t const start,
                              size_t const num_trits);

/// Returns an array of trits regardless of the current memory storage
/// scheme
/// @param[in] trits - an array of individual trits
/// @param[in] trit_array - the array of packed trits
/// @param[in] num_trits - the number of trits to extract
/// @return size_t - the number of trits extracted
size_t flex_trit_array_to_int8(trit_t *const trits, size_t const to_len,
                               flex_trit_t const *const trit_array,
                               size_t const len, size_t const num_trits);

/// Returns an array of trits packed in the current memory storage scheme
/// @param[in] trit_array - the array of packed trits
/// @param[in] trits - an array of individual trits
/// @param[in] num_trits - the number of trits to pack
/// @return size_t - the number of trits packed
size_t int8_to_flex_trit_array(flex_trit_t *const to_trit_array,
                               size_t const to_len, trit_t const *const trits,
                               size_t const len, size_t const num_trits);

/// Returns an array of trytes.
/// @param[in] trytes - an array to store trytes
/// @param[in] to_len - the size of the array to store trytes
/// @param[in] trit_array - the array of packed trits
/// @param[in] len - the size of the trit array
/// @param[in] num_trits - the number of trits to pack
/// @return size_t - the number of trits decoded
size_t flex_trit_to_tryte(tryte_t *trytes, size_t to_len,
                          const flex_trit_t *trit_array, size_t len,
                          size_t num_trits);

/// Returns an array of flex_trits.
/// @param[in] trit_array - the array of packed trits
/// @param[in] to_len - the number of trits in the flex_trits array
/// @param[in] trytes - an array of trytes
/// @param[in] len - the number of trytes in the trytes array
/// @param[in] num_trytes - the size of trytes to unpack
/// @return size_t - the number of trytes decoded
size_t tryte_to_flex_trit(flex_trit_t *trit_array, size_t to_len,
                          const tryte_t *trytes, size_t len, size_t num_trytes);
/***********************************************************************************************************
 * Trits array
 ***********************************************************************************************************/
/// A trit array represents an array of trits packed according to the
/// current memory model.
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
size_t trit_array_bytes_for_trits(size_t const num_trits);

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
/// Returns the trit at a given index in an array of trits
/// @param[in] trit_array - a pointer to a trit_array_t
/// @param[in] index - the index of the trit to access
/// @return trit_t - the trit at the given index
static inline trit_t trit_array_at(trit_array_p const trit_array,
                                   size_t const index) {
  return flex_trit_array_at(trit_array->trits, trit_array->num_trits, index);
}

/// Set the trit at a given index in an array of trits to the given value
/// @param[in] trit_array - a pointer to a trit_array_t
/// @param[in] index - the index of the trit to access
/// @param[in] trit - the trit value to set
static inline uint8_t trit_array_set_at(trit_array_p const trit_array,
                                        size_t const index, trit_t const trit) {
  return flex_trit_array_set_at(trit_array->trits, trit_array->num_trits, index,
                                trit);
}

void trit_array_set_trits(trit_array_p const trit_array,
                          flex_trit_t *const trits, size_t const num_trits);

/// Returns a new trit_array containing a portion of length num_trits of the
/// trits of another trit_array, from index start.
/// @param[in] trit_array - the original trit_array
/// @param[in] to_trit_array - the new trit_array
/// @param[in] start - the start index in the original array
/// @param[in] num_trits - the number of trits to copy over
/// @return trit_array_p - the new trit_array
trit_array_p trit_array_slice(trit_array_p const trit_array,
                              trit_array_p const to_trit_array,
                              size_t const start, size_t const num_trits);

/// Inserts the contents of an array into the receiver starting at a given
/// index.
/// @param[in] trit_array - the array to insert into
/// @param[in] from_trit_array - the array containing the trits to copy over
/// @param[in] start - the start index in the target array
/// @param[in] num_trits - the number of trits to copy over
/// @return trit_array_p - the receiver
trit_array_p trit_array_insert(trit_array_p const trit_array,
                               trit_array_p const from_trit_array,
                               size_t const start, size_t const num_trits);

/// Returns an array of trits regardless of the current memory storage scheme
/// @param[in] trit_array - the array of packed trits
/// @param[in] trits - an array of individual trits
/// @return trit_t - the array of individual trits
trit_t *trit_array_to_int8(trit_array_p const trit_array, trit_t *const trits,
                           size_t const len);

#if !defined(NO_DYNAMIC_ALLOCATION)
/***********************************************************************************************************
 * Constructor
 ***********************************************************************************************************/
/// Returns a new trit_array that can accomodate num_trits trits
/// @param[in] num_trits - the number of trits to store
/// @return trit_array_p - the new trit_array
trit_array_p trit_array_new(size_t const num_trits);

/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
void trit_array_free(trit_array_p const trit_array);

#endif  // NO_DYNAMIC_ALLOCATION

/// Macro to declare a new trit_array.
/// @param[in] NAME - the name of the variable to declare
/// @param[in] NUM_TRITS - the number of trits the trit_array can store
#define TRIT_ARRAY_DECLARE(NAME, NUM_TRITS)                           \
  size_t NAME##_num_bytes = trit_array_bytes_for_trits(NUM_TRITS);    \
  flex_trit_t NAME##_trits[NAME##_num_bytes];                         \
  memset(&NAME##_trits, 0, NAME##_num_bytes);                         \
  struct _trit_array NAME = {(flex_trit_t *)&NAME##_trits, NUM_TRITS, \
                             NAME##_num_bytes};

/// Macro to declare a new trit_array and initialize it with an array of trits
/// stored according to the current memory model.
/// @param[in] NAME - the name of the varialbe to declare
/// @param[in] NUM_TRITS - the number of trits the trit_array can store
/// @param[in] TRITS - the initial trits packed in to the current memory model
#define TRIT_ARRAY_MAKE(NAME, NUM_TRITS, TRITS)                      \
  size_t NAME##_num_bytes = trit_array_bytes_for_trits(NUM_TRITS);   \
  flex_trit_t NAME##_trits[] = {TRITS};                              \
  struct _trit_array NAME = {(flex_trit_t *)NAME##_trits, NUM_TRITS, \
                             NAME##_num_bytes};

#endif
#ifdef __cplusplus
}
#endif
