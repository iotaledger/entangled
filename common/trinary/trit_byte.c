/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>

#include "common/defs.h"
#include "common/trinary/trit_byte.h"
#include "utils/macros.h"

static const byte_t byte_radix[] = {1, 3, 9, 27, 81};

void trits_to_bytes(trit_t *trits, byte_t *bytes, size_t num_trits) {
  if (num_trits == 0) {
    return;
  }
  size_t end = MIN(num_trits, NUMBER_OF_TRITS_IN_A_BYTE);
  bytes[0] = trits_to_byte(trits, 0, end);
  trits_to_bytes(&trits[end], &bytes[1], num_trits - end);
}

size_t min_bytes(size_t const num_trits) {
  return (num_trits + NUMBER_OF_TRITS_IN_A_BYTE - 1) /
         NUMBER_OF_TRITS_IN_A_BYTE;
}

byte_t trits_to_byte(trit_t const *const trits, byte_t const cum,
                     size_t const num_trits) {
  assert(num_trits <= NUMBER_OF_TRITS_IN_A_BYTE);
  if (num_trits == 0) {
    return cum;
  }
  byte_t byte = cum * RADIX + trits[num_trits - 1];
  return trits_to_byte(trits, byte, num_trits - 1);
}

void _byte_to_trits(byte_t byte, trit_t *const trits, size_t j, size_t i) {
  while (j < -1) {
    trit_t trit = 0;
    if (byte > (byte_radix[j] >> 1)) {
      byte -= byte_radix[j];
      trit = 1;
    } else if (byte < -(byte_radix[j] >> 1)) {
      byte += byte_radix[j];
      trit = -1;
    }
    if (j == i) {
      trits[i] = trit;
      i -= 1;
    }
    j -= 1;
  }
}

void bytes_to_trits(byte_t const *const bytes, size_t const num_bytes,
                    trit_t *const trits, size_t const num_trits) {
  assert(num_trits <= NUMBER_OF_TRITS_IN_A_BYTE * num_bytes);
  if (num_bytes == 0 || num_trits == 0) {
    return;
  }
  size_t end = MIN(num_trits, NUMBER_OF_TRITS_IN_A_BYTE);
  _byte_to_trits(*bytes, trits, NUMBER_OF_TRITS_IN_A_BYTE - 1, end - 1);
  bytes_to_trits(&bytes[1], num_bytes - 1, &trits[end], num_trits - end);
}

void byte_to_trits(byte_t byte, trit_t *const trit, size_t const num_trits) {
  assert(num_trits <= NUMBER_OF_TRITS_IN_A_BYTE);
  if (num_trits == 0) {
    return;
  }
  _byte_to_trits(byte, trit, NUMBER_OF_TRITS_IN_A_BYTE - 1, num_trits - 1);
}
