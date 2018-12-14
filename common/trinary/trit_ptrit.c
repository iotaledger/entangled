/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "trit_ptrit.h"

#define HIGH_BITS 0xFFFFFFFFFFFFFFFF
#define LOW_BITS 0x0000000000000000

void trits_to_ptrits(trit_t const *const trits, ptrit_t *const ptrits,
                     size_t const index, size_t const length) {
  size_t j = 0;
  ptrit_s lmask, hmask;

  if (length == 0) {
    return;
  }

  for (; j < length; j++) {
    switch (trits[j]) {
      case 0:
        lmask = (1uLL << index);
        hmask = (1uLL << index);
        break;
      case 1:
        hmask = (1uLL << index);
        lmask = 0;
        break;
      default:
        lmask = (1uLL << index);
        hmask = 0;
    }

    ptrits[j].low |= lmask;
    ptrits[j].high |= hmask;
  }
}

void trits_to_ptrits_fill(trit_t const *const trits, ptrit_t *const ptrits,
                          size_t const length) {
  if (length == 0) {
    return;
  }

  switch (*trits) {
    case 0:
      ptrits->low = HIGH_BITS;
      ptrits->high = HIGH_BITS;
      break;
    case 1:
      ptrits->low = LOW_BITS;
      ptrits->high = HIGH_BITS;
      break;
    default:
      ptrits->low = HIGH_BITS;
      ptrits->high = LOW_BITS;
  }
  trits_to_ptrits_fill(&trits[1], &ptrits[1], length - 1);
}

void ptrits_to_trits(ptrit_t const *const ptrits, trit_t *const trits,
                     size_t const index, size_t length) {
  size_t j = 0;
  if (length == 0) {
    return;
  }

  ptrit_s off = (1uLL << index);

  for (; j < length; j++) {
    trits[j] = (ptrits[j].low & off) ? ((ptrits[j].high & (off)) ? 0 : -1) : 1;
  }
}
