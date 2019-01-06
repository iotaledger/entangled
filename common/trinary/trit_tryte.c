/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/defs.h"
#include "common/trinary/trit_tryte.h"

#define TRYTE_SPACE 27
#define TRITS_TO_TRYTES_MAP                                                 \
  {0, 0, 0}, {1, 0, 0}, {-1, 1, 0}, {0, 1, 0}, {1, 1, 0}, {-1, -1, 1},      \
      {0, -1, 1}, {1, -1, 1}, {-1, 0, 1}, {0, 0, 1}, {1, 0, 1}, {-1, 1, 1}, \
      {0, 1, 1}, {1, 1, 1}, {-1, -1, -1}, {0, -1, -1}, {1, -1, -1},         \
      {-1, 0, -1}, {0, 0, -1}, {1, 0, -1}, {-1, 1, -1}, {0, 1, -1},         \
      {1, 1, -1}, {-1, -1, 0}, {0, -1, 0}, {1, -1, 0}, {                    \
    -1, 0, 0                                                                \
  }

static const trit_t TRYTE_TO_TRITS_MAPPINGS[TRYTE_SPACE]
                                           [NUMBER_OF_TRITS_IN_A_TRYTE] = {
                                               TRITS_TO_TRYTES_MAP};

size_t num_trytes_for_trits(size_t num_trits) {
  return (num_trits + NUMBER_OF_TRITS_IN_A_TRYTE - 1) /
         NUMBER_OF_TRITS_IN_A_TRYTE;
}

trit_t get_trit_at(tryte_t *const trytes, size_t const length, size_t index) {
  size_t tindex = index / 3U;
  if (tindex >= length) {
    return 0;
  }
  tryte_t tryte = trytes[tindex];
  size_t cindex = tryte == '9' ? 0 : tryte - '@';
  tindex = index % 3U;
  return TRYTE_TO_TRITS_MAPPINGS[cindex][tindex];
}

uint8_t set_trit_at(tryte_t *const trytes, size_t const length, size_t index,
                    trit_t trit) {
  size_t tindex = index / 3U;
  if (tindex >= length) {
    return 0;
  }
  tryte_t tryte = trytes[tindex];
  size_t cindex = tryte == '9' ? 0 : tryte - '@';
  trit_t trits[3];
  if (cindex > 26) {
    // uninitialized
    memcpy(trits, TRYTE_TO_TRITS_MAPPINGS[0], NUMBER_OF_TRITS_IN_A_TRYTE);
  } else {
    memcpy(trits, TRYTE_TO_TRITS_MAPPINGS[cindex], NUMBER_OF_TRITS_IN_A_TRYTE);
  }
  size_t uindex = index % 3U;
  trits[uindex] = trit;
  tryte = trits[0] + trits[1] * 3 + trits[2] * 9;
  if (tryte < 0) {
    tryte += TRYTE_SPACE;
  }
  trytes[tindex] = TRYTE_ALPHABET[(size_t)tryte];
  return 1;
}

void trits_to_trytes(trit_t const *const trits, tryte_t *const trytes,
                     size_t const length) {
  int k = 0;

  for (size_t i = 0, j = 0; i < length; i += 3, j++) {
    k = 0;
    for (size_t l = length - i < NUMBER_OF_TRITS_IN_A_TRYTE
                        ? length - i
                        : NUMBER_OF_TRITS_IN_A_TRYTE;
         l-- > 0;) {
      k *= 3;
      k += trits[i + l];
    }
    if (k < 0) {
      k += TRYTE_SPACE;
    }
    trytes[j] = TRYTE_ALPHABET[k];
  }
}

void trytes_to_trits(tryte_t const *const trytes, trit_t *const trits,
                     size_t const length) {
  if (length == 0) {
    return;
  }

  for (size_t i = 0, j = 0; i < length; i++, j += 3) {
    memcpy(trits + j,
           TRYTE_TO_TRITS_MAPPINGS[trytes[i] == '9' ? 0 : trytes[i] - '@'],
           NUMBER_OF_TRITS_IN_A_TRYTE);
  }
}
