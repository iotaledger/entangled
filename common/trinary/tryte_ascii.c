/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/tryte_ascii.h"
#include "common/defs.h"

void ascii_to_trytes(char const *const input, tryte_t *const output) {
  int j = 0, dec = 0, first = 0, second = 0;

  for (size_t i = 0; input[i]; i++) {
    dec = input[i];
    first = dec % TRYTE_SPACE_SIZE;
    second = (dec - first) / TRYTE_SPACE_SIZE;
    output[j++] = TRYTE_ALPHABET[first];
    output[j++] = TRYTE_ALPHABET[second];
  }
}

void trytes_to_ascii(tryte_t const *const input, size_t const input_size, char *const output) {
  for (size_t i = 0; i < input_size; i += 2) {
    output[i / 2] = INDEX_OF_TRYTE(input[i]) + INDEX_OF_TRYTE(input[i + 1]) * TRYTE_SPACE_SIZE;
  }
}
