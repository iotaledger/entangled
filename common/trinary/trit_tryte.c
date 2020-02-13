/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/trinary/trit_tryte.h"
#if defined(__SSE4_2__)
#include "common/trinary/trit_tryte_sse42.h"
#endif

trit_t get_trit_at(tryte_t const *const trytes, size_t const length, size_t const index) {
  size_t tindex = index / 3U;
  if (tindex >= length) {
    return 0;
  }
  tryte_t tryte = trytes[tindex];
  size_t cindex = INDEX_OF_TRYTE(tryte);
  tindex = index % 3U;
  return TRYTES_TRITS_LUT[cindex][tindex];
}

uint8_t set_trit_at(tryte_t *const trytes, size_t const length, size_t const index, trit_t const trit) {
  size_t tindex = index / 3U;
  if (tindex >= length) {
    return 0;
  }
  tryte_t tryte = trytes[tindex];
  size_t cindex = INDEX_OF_TRYTE(tryte);
  trit_t trits[3];
  if (cindex > 26) {
    // uninitialized
    memcpy(trits, TRYTES_TRITS_LUT[0], NUMBER_OF_TRITS_IN_A_TRYTE);
  } else {
    memcpy(trits, TRYTES_TRITS_LUT[cindex], NUMBER_OF_TRITS_IN_A_TRYTE);
  }
  size_t uindex = index % 3U;
  trits[uindex] = trit;
  tryte = trits[0] + trits[1] * 3 + trits[2] * 9;
  if (tryte < 0) {
    tryte += TRYTE_SPACE_SIZE;
  }
  trytes[tindex] = TRYTE_ALPHABET[(size_t)tryte];
  return 1;
}

void trits_to_trytes(trit_t const *const trits, tryte_t *const trytes, size_t const length) {
  if (length == 0) {
    return;
  }

#if defined(__SSE4_2__)
  if (length >= TRITS_TO_TRYTES_THRESHOLD) {
    trits_to_trytes_sse42(trits, trytes, length);
    return;
  }
#endif
  int k = 0;

  for (size_t i = 0, j = 0; i < length; i += RADIX, j++) {
    k = 0;
    for (size_t l = length - i < NUMBER_OF_TRITS_IN_A_TRYTE ? length - i : NUMBER_OF_TRITS_IN_A_TRYTE; l-- > 0;) {
      k *= RADIX;
      k += trits[i + l];
    }
    if (k < 0) {
      k += TRYTE_SPACE_SIZE;
    }
    trytes[j] = TRYTE_ALPHABET[k];
  }
}

void trytes_to_trits(tryte_t const *const trytes, trit_t *const trits, size_t const length) {
  if (length == 0) {
    return;
  }

#if defined(__SSE4_2__)
  if (length >= TRYTES_TO_TRITS_THRESHOLD) {
    trytes_to_trits_sse42(trytes, trits, length);
    return;
  }
#endif
  for (size_t i = 0, j = 0; i < length; i++, j += RADIX) {
    memcpy(trits + j, TRYTES_TRITS_LUT[INDEX_OF_TRYTE(trytes[i])], NUMBER_OF_TRITS_IN_A_TRYTE);
  }
}
