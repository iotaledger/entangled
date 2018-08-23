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

#include <stdbool.h>
#include <string.h>
#include "common/trinary/flex_trit.h"

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
  return flex_trits_at(trit_array->trits, trit_array->num_trits, index);
}

/// Set the trit at a given index in an array of trits to the given value
/// @param[in] trit_array - a pointer to a trit_array_t
/// @param[in] index - the index of the trit to access
/// @param[in] trit - the trit value to set
static inline uint8_t trit_array_set_at(trit_array_p const trit_array,
                                        size_t const index, trit_t const trit) {
  return flex_trits_set_at(trit_array->trits, trit_array->num_trits, index,
                           trit);
}

void trit_array_set_trits(trit_array_p const trit_array,
                          flex_trit_t *const trits, size_t const num_trits);

bool trit_array_is_null(trit_array_p const trit_array);

static inline void trit_array_set_null(trit_array_p const trit_array) {
  memset(trit_array->trits, FLEX_TRIT_NULL_VALUE, trit_array->num_bytes);
}

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
#endif  // __COMMON_TRINARY_TRIT_ARRAY_H_
