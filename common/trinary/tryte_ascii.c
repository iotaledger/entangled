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

  for (int i = 0; input[i]; i++) {
    dec = input[i];
    first = dec % 27;
    second = (dec - first) / 27;
    output[j++] = TRYTE_ALPHABET[first];
    output[j++] = TRYTE_ALPHABET[second];
  }
}

static int index_of_tryte(tryte_t tryte) {
  if (tryte == '9') {
    return 0;
  }
  return tryte - 'A' + 1;
}

void trytes_to_ascii(tryte_t const *const input, char *const output) {
  for (int i = 0; input[i]; i += 2) {
    output[i / 2] =
        index_of_tryte(input[i]) + index_of_tryte(input[i + 1]) * 27;
  }
}
