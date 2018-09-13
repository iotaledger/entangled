/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/tryte_ascii.h"

#define TRYTE_ALPHABET "9ABCDEFGHIJKLMNOPQRSTUVWXYZ"

void ascii_to_trytes(char const *const input, tryte_t *const output) {
  int j = 0, dec = 0, first = 0, second = 0;

  for (int i = 0; input[i]; i++) {
    dec = input[i];
    first = dec % 27;
    second = (dec - first) / 27;
    output[j++] = TRYTE_ALPHABET[first];
    output[j++] = TRYTE_ALPHABET[second];
  }
}
