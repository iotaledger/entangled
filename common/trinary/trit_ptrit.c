/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/trit_ptrit.h"

void trits_to_ptrits(trit_t const *const trits, ptrit_t *const ptrits, size_t const index, size_t const length) {
  size_t j = 0;

  if (length == 0) {
    return;
  }

  for (; j < length; j++) {
    switch (trits[j]) {
      case 0:
        ptrits[j].low |= (1uLL << index);
        ptrits[j].high |= (1uLL << index);
        break;
      case 1:
        ptrits[j].high |= (1uLL << index);
        break;
      default:
        ptrits[j].low |= (1uLL << index);
        break;
    }
  }
}

void trits_to_ptrits_fill(trit_t const *const trits, ptrit_t *const ptrits, size_t const length) {
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

void ptrits_to_trits(ptrit_t const *const ptrits, trit_t *const trits, size_t const index, size_t const length) {
  size_t j = 0;

  if (length == 0) {
    return;
  }

  for (; j < length; j++) {
    int h = (ptrits[j].high >> index) & 1;
    int l = (ptrits[j].low >> index) & 1;

    trits[j] = l ? (h ? 0 : -1) : 1;
  }
}
