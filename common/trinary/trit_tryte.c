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
  memcpy(trits, TRYTE_TO_TRITS_MAPPINGS[cindex], NUMBER_OF_TRITS_IN_A_TRYTE);
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
  int j = 0,
      end = length < NUMBER_OF_TRITS_IN_A_TRYTE ? length
                                                : NUMBER_OF_TRITS_IN_A_TRYTE,
      i = end;
  for (; i-- > 0;) {
    j *= 3;
    j += trits[i];
  }
  // int j = trits[0] + trits[1] * 3 + trits[2] * 9;
  if (j < 0) {
    j += TRYTE_SPACE;
  }
  trytes[0] = TRYTE_ALPHABET[(size_t)j];
  if (length <= NUMBER_OF_TRITS_IN_A_TRYTE) {
    return;
  }
  trits_to_trytes(&trits[NUMBER_OF_TRITS_IN_A_TRYTE], &trytes[1],
                  length - NUMBER_OF_TRITS_IN_A_TRYTE);
}

void trytes_to_trits(tryte_t const *const tryte, trit_t *const trits,
                     size_t const length) {
  if (length == 0) {
    return;
  }
  memcpy(trits,
         TRYTE_TO_TRITS_MAPPINGS[strchr(TRYTE_ALPHABET, tryte[0]) -
                                 TRYTE_ALPHABET],
         NUMBER_OF_TRITS_IN_A_TRYTE * sizeof(trit_t));
  trytes_to_trits(&tryte[1], &trits[NUMBER_OF_TRITS_IN_A_TRYTE], length - 1);
}
