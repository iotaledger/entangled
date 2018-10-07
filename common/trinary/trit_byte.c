/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "trit_byte.h"

#define NUMBER_OF_TRITS_IN_A_BYTE 5
#define RADIX 3
static const byte_t byte_radix[] = {1, 3, 9, 27, 81};

void trits_to_bytes(trit_t *trits, byte_t *bytes, size_t size) {
  if (size <= 0) {
    return;
  }
  size_t end;
  if (size < NUMBER_OF_TRITS_IN_A_BYTE) {
    end = size;
  } else {
    end = NUMBER_OF_TRITS_IN_A_BYTE;
  }
  bytes[0] = trits_to_byte(trits, 0, end - 1);
  trits_to_bytes(&trits[end], &bytes[1], size - end);
}

size_t min_bytes(size_t const trits_length) {
  return (trits_length + NUMBER_OF_TRITS_IN_A_BYTE - 1) /
         NUMBER_OF_TRITS_IN_A_BYTE;
}

byte_t trits_to_byte(trit_t const *const trits, byte_t const cum,
                     size_t const i) {
  if (i == 0) {
    return cum * RADIX + trits[i];
  }
  return trits_to_byte(trits, cum * RADIX + trits[i], i - 1);
}

void _byte_to_trits(byte_t byte, trit_t *const trits, size_t const j, size_t const i) {
  trit_t trit;
  if (byte > (byte_radix[j] >> 1)) {
    byte -= byte_radix[j];
    trit = 1;
  } else if (byte < -(byte_radix[j] >> 1)) {
    byte += byte_radix[j];
    trit = -1;
  } else {
    trit = 0;
  }
  if (j == i) {
    trits[i] = trit;
  }
  if (j == 0) {
    return;
  }
  _byte_to_trits(byte, trits, j - 1, j == i ? i - 1 : i);
}

void bytes_to_trits(byte_t const *const byte, size_t const n_bytes,
                    trit_t *const trit, size_t const n_trits) {
  if (n_bytes == 0 || n_trits == 0) {
    return;
  }
  size_t end =
      n_trits < NUMBER_OF_TRITS_IN_A_BYTE ? n_trits : NUMBER_OF_TRITS_IN_A_BYTE;
  _byte_to_trits(*byte, trit, NUMBER_OF_TRITS_IN_A_BYTE - 1, end - 1);
  bytes_to_trits(&byte[1], n_bytes - 1, &trit[end], n_trits - end);
}

void byte_to_trits(byte_t byte, trit_t *const trit, size_t const i) {
  _byte_to_trits(byte, trit, NUMBER_OF_TRITS_IN_A_BYTE, i);
}
