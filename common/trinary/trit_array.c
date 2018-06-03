/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include "common/trinary/trit_array.h"
#include "common/trinary/trit_byte.h"

#if !defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE) && \
    !defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
#define TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE
#endif

/***********************************************************************************************************
 * Trits
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
// struct _trit_array {...}

/***********************************************************************************************************
 * Public interface
 ***********************************************************************************************************/
size_t trit_array_bytes_for_trits(size_t num_trits) {
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  return num_trits;
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)  
  return (num_trits + 3) / 4;
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
  return min_bytes(num_trits);
#endif
}

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
void trit_array_set_trits(trit_array_p trit_array, char *trits, size_t num_trits) {
  if (trit_array->dynamic) {
    free(trit_array->trits);
    trit_array->dynamic = 0;
  }
  trit_array->trits = trits;
  trit_array->num_trits = num_trits;
  trit_array->num_bytes = trit_array_bytes_for_trits(num_trits);
}

trit_array_p trit_array_slice(trit_array_p trit_array, trit_array_p *to_trit_array, size_t start, size_t num_trits) {
  trit_array_p new_trit_array;
  if (!to_trit_array) {
    new_trit_array = trit_array_new(num_trits);
  }
  else {
    new_trit_array = *to_trit_array;
  }
  new_trit_array->num_trits = num_trits;
  new_trit_array->num_bytes = trit_array_bytes_for_trits(num_trits);
#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
  memcpy(new_trit_array->trits, trit_array->trits + start, num_trits);
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
  byte_t buffer;
  uint8_t tshift = (start % 4U) << 1U;
  uint8_t rshift = (8U - tshift) % 8U;
  size_t index = start / 4U;
  size_t max_index = (start + num_trits - 1) / 4U;
  // Calculate the number of bytes to copy over
  for (int i = index, j = 0; i < index + new_trit_array->num_bytes; i++, j++) {
    buffer = trit_array->trits[i];
    buffer = buffer >> tshift;
    if (rshift && i < max_index) {
      buffer |= (trit_array->trits[i + 1] << rshift);
    }
    new_trit_array->trits[j] = buffer;
  }
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
  byte_t buffer = 0;
  trit_t trits[10];
  size_t index = start / 5U;
  size_t offset = start % 5U;
  size_t max_index = (start + num_trits - 1) / 5U;
  for (int i = index, j = 0; i < index + new_trit_array->num_bytes; i++, j++) {
    bytes_to_trits(((byte_t *)trit_array->trits + i), 1, trits, 5);
    if (offset && i < max_index) {
      bytes_to_trits(((byte_t *)trit_array->trits + i + 1), 1, ((trit_t *)trits + 5), 5);
    }
    new_trit_array->trits[j] = trits_to_byte(trits + offset, buffer, 5);
  }
#endif
  return new_trit_array;
}

/***********************************************************************************************************
 * Constructor
 ***********************************************************************************************************/
trit_array_p trit_array_new(size_t num_trits) {
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
void trit_array_free(trit_array_p trit_array) {
  if (trit_array) {
    if (trit_array->dynamic) {
      free(trit_array->trits);
    }
  }
  free(trit_array);
}
