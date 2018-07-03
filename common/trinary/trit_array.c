/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled/
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/trit_array.h"
#include <stdlib.h>
#include <string.h>
#include "common/trinary/trit_byte.h"

size_t flex_trit_array_slice(flex_trit_t *const to_trit_array,
                             size_t const to_len,
                             flex_trit_t const *const trit_array,
                             size_t const len, size_t const start,
                             size_t const num_trits) {
  // Bounds checking
  if (num_trits > to_len || (start + num_trits) > len) {
    return 0;
  }
  size_t num_bytes = trit_array_bytes_for_trits(num_trits);
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  memcpy(to_trit_array, trit_array + start, num_trits);
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  uint8_t buffer;
  uint8_t tshift = (start & 3) << 1U;
  uint8_t rshift = (8U - tshift) % 8U;
  size_t index = start >> 2U;
  size_t end_index = (start + num_trits - 1) >> 2U;
  size_t i, j;
  // Calculate the number of bytes to copy over
  for (i = index, j = 0; i < index + num_bytes; i++, j++) {
    buffer = trit_array[i];
    buffer = buffer >> tshift;
    if (i < end_index) {
      if (rshift) {
        buffer |= (trit_array[i + 1] << rshift);
      }
    } else {
      uint8_t residual = (num_trits & 3);
      if (residual) {
        uint8_t shift = (4 - residual) << 1U;
        buffer = (uint8_t)(buffer << shift) >> shift;
      }
    }
    to_trit_array[j] = buffer;
  }
  // There is a risk of noise after the last trit so we need to clean up
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
  trit_t trits[10];
  size_t index = start / 5U;
  size_t offset = start % 5U;
  size_t end_index = (start + num_trits - 1) / 5U;
  size_t i, j, tlen = 5;
  for (i = index, j = 0; i < index + num_bytes; i++, j++) {
    bytes_to_trits(((byte_t *)trit_array + i), 1, trits, 5);
    if (i < end_index) {
      if (offset) {
        bytes_to_trits(((byte_t *)trit_array + i + 1), 1, ((trit_t *)trits + 5),
                       5);
      }
    } else if (num_bytes * 5 > num_trits) {
      tlen = (num_trits - num_bytes * 5 + 5);
    }
    to_trit_array[j] = trits_to_byte(trits + offset, 0, tlen - 1);
  }
#endif
  return num_bytes;
}

size_t flex_trit_array_insert(flex_trit_t *const to_trit_array,
                              size_t const to_len,
                              flex_trit_t const *const trit_array,
                              size_t const len, size_t const start,
                              size_t const num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len) {
    return 0;
  }
  for (size_t i = 0; i < num_trits; i++) {
    flex_trit_array_set_at(to_trit_array, to_len, start + i,
                           flex_trit_array_at(trit_array, len, i));
  }
  return num_trits;
}

size_t flex_trit_array_to_int8(trit_t *const trits, size_t const to_len,
                               flex_trit_t const *const trit_array,
                               size_t const len, size_t const num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len) {
    return 0;
  }
  size_t num_bytes = trit_array_bytes_for_trits(num_trits);
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  memcpy(trits, trit_array, num_bytes);
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  for (size_t i = 0; i < num_trits; i++) {
    trits[i] = flex_trit_array_at(trit_array, len, i);
  }
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
  bytes_to_trits((flex_trit_t *)trit_array, num_bytes, trits, num_trits);
#endif
  return num_bytes;
}

size_t int8_to_flex_trit_array(flex_trit_t *const to_trit_array,
                               size_t const to_len, trit_t const *const trits,
                               size_t const len, size_t const num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len) {
    return 0;
  }
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  memcpy(to_trit_array, trits, num_trits);
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  for (size_t i = 0; i < num_trits; i++) {
    flex_trit_array_set_at(to_trit_array, to_len, i, trits[i]);
  }
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
  trits_to_bytes((trit_t *)trits, to_trit_array, num_trits);
#endif
  return num_trits;
}

/***********************************************************************************************************
 * Trits
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _trit_array {...}

/***********************************************************************************************************
 * Public interface
 ***********************************************************************************************************/
size_t trit_array_bytes_for_trits(size_t const num_trits) {
  return flex_trits_num_for_trits(num_trits);
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

trit_array_p trit_array_slice(trit_array_p trit_array,
                              trit_array_p to_trit_array, size_t start,
                              size_t num_trits) {
#if !defined(NO_DYNAMIC_ALLOCATION)
  to_trit_array = to_trit_array ? to_trit_array : trit_array_new(num_trits);
#endif  // NO_DYNAMIC_ALLOCATION
  to_trit_array->num_trits = num_trits;
  to_trit_array->num_bytes = flex_trit_array_slice(
      to_trit_array->trits, to_trit_array->num_trits, trit_array->trits,
      trit_array->num_trits, start, num_trits);
  return to_trit_array;
}

trit_t *trit_array_to_int8(trit_array_p const trit_array, trit_t *const trits,
                           size_t const len) {
  flex_trit_array_to_int8(trits, len, trit_array->trits, trit_array->num_trits,
                          trit_array->num_trits);
  return trits;
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
  memset(trit_array->trits, 0, trit_array->num_bytes);
  trit_array->dynamic = 1;
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
