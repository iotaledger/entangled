/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/trit_array.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/***********************************************************************************************************
 * Trit Array
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _trit_array {...}

/***********************************************************************************************************
 * Public interface
 ***********************************************************************************************************/
size_t trit_array_bytes_for_trits(size_t const num_trits) {
  return NUM_FLEX_TRITS_FOR_TRITS(num_trits);
}

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
void trit_array_set_trits(trit_array_p const trit_array,
                          flex_trit_t *const trits, size_t const num_trits) {
#if !defined(NO_DYNAMIC_ALLOCATION)
  if (trit_array->dynamic) {
    free(trit_array->trits);
    trit_array->dynamic = 0;
  }
#endif  // NO_DYNAMIC_ALLOCATION
  trit_array->trits = trits;
  trit_array->num_trits = num_trits;
  trit_array->num_bytes = trit_array_bytes_for_trits(num_trits);
}

bool trit_array_is_null(trit_array_p const trit_array) {
  for (size_t i = 0; i < trit_array->num_bytes; i++) {
    if (trit_array->trits[i] != FLEX_TRIT_NULL_VALUE) {
      return false;
    }
  }
  return true;
}

trit_array_p trit_array_slice(trit_array_p trit_array,
                              trit_array_p to_trit_array, size_t start,
                              size_t num_trits) {
#if !defined(NO_DYNAMIC_ALLOCATION)
  to_trit_array = to_trit_array ? to_trit_array : trit_array_new(num_trits);
#endif  // NO_DYNAMIC_ALLOCATION
  to_trit_array->num_trits = num_trits;
  to_trit_array->num_bytes = flex_trits_slice(
      to_trit_array->trits, to_trit_array->num_trits, trit_array->trits,
      trit_array->num_trits, start, num_trits);
  return to_trit_array;
}

trit_array_p trit_array_slice_at_most(trit_array_p const trit_array,
                                      trit_array_p const to_trit_array,
                                      const size_t start, size_t num_trits) {
  if (trit_array->num_trits - start < num_trits) {
    num_trits = trit_array->num_trits - start;
  }
  trit_array_slice(trit_array, to_trit_array, start, num_trits);
}

trit_array_p trit_array_insert(trit_array_p const trit_array,
                               trit_array_p const from_trit_array,
                               size_t const start, size_t const num_trits) {
  flex_trits_insert(trit_array->trits, trit_array->num_trits,
                    from_trit_array->trits, from_trit_array->num_trits, start,
                    num_trits);
  return trit_array;
}

trit_array_p trit_array_insert_from_pos(trit_array_p const dst_trits,
                                        trit_array_p const src_trits,
                                        size_t const src_start_pos,
                                        size_t const dst_start_pos,
                                        size_t const num_trits) {
  flex_trits_insert_from_pos(dst_trits->trits, dst_trits->num_trits,
                             src_trits->trits, src_trits->num_trits,
                             src_start_pos, dst_start_pos, num_trits);
  return dst_trits;
}

trit_t *trit_array_to_int8(trit_array_p const trit_array, trit_t *const trits,
                           size_t const len) {
  flex_trits_to_trits(trits, len, trit_array->trits, trit_array->num_trits,
                      trit_array->num_trits);
  return trits;
}

trit_array_p trit_array_set_range(trit_array_p const trits, size_t start,
                                  size_t end, trit_t value) {
  assert(start < trits->num_trits && end < trits->num_trits);
  for (size_t idx = start; idx < end; ++idx) {
    trit_array_set_at(trits, idx, value);
  }
  return trits;
}

bool trit_array_equal(trit_array_p const lhs, trit_array_p const rhs) {
  if (lhs->num_trits != rhs->num_trits) {
    return false;
  }
  return memcmp(lhs->trits, rhs->trits, lhs->num_bytes) == 0;
}

#if !defined(NO_DYNAMIC_ALLOCATION)
/***********************************************************************************************************
 * Constructor
 ***********************************************************************************************************/
trit_array_p trit_array_new(size_t const num_trits) {
  trit_array_p trit_array;
  trit_array = (trit_array_p)malloc(sizeof(struct _trit_array));
  if (!trit_array) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  memset(trit_array, 0, sizeof(struct _trit_array));
  trit_array->num_trits = num_trits;
  trit_array->num_bytes = trit_array_bytes_for_trits(num_trits);
  trit_array->trits = malloc(trit_array->num_bytes);
  if (!trit_array->trits) {
    trit_array_free(trit_array);
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  trit_array_set_null(trit_array);
  trit_array->dynamic = 1;
  return trit_array;
}

trit_array_p trit_array_new_from_trytes(tryte_t const *const trytes) {
  size_t num_trytes = strlen((char *)trytes);
  size_t num_trits = num_trytes * 3;
  trit_array_p trit_array;
  if ((trit_array = trit_array_new(num_trits)) == NULL) {
    return NULL;
  }
  flex_trits_from_trytes(trit_array->trits, num_trits, trytes, num_trytes,
                         num_trytes);
  return trit_array;
}

/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
void trit_array_free(trit_array_p const trit_array) {
  if (trit_array) {
    if (trit_array->dynamic) {
      free(trit_array->trits);
    }
  }
  free(trit_array);
}

#endif  // NO_DYNAMIC_ALLOCATION
