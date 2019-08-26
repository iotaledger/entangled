/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/trit_ptrit.h"

void trits_to_ptrits(trit_t const *const trits, ptrit_t *const ptrits, size_t const index, size_t const length) {
  ptrits_set_slice(length, ptrits, index, trits);
}

void trits_to_ptrits_fill(trit_t const *const trits, ptrit_t *const ptrits, size_t const length) {
  ptrits_fill(length, ptrits, trits);
}

void ptrits_to_trits(ptrit_t const *const ptrits, trit_t *const trits, size_t const index, size_t const length) {
  ptrits_get_slice(length, trits, ptrits, index);
}
