/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/trinary/add.h"
#include "mam/prototype/mask.h"

void mask(trit_t *const dest, trit_t const *const message, size_t const length, Curl *const c) {
  size_t chunk_length;
  trit_t chunk[HASH_LENGTH_TRIT];
  for (size_t i = 0; i < length; i += HASH_LENGTH_TRIT) {
    chunk_length = length - i < HASH_LENGTH_TRIT ? length - i : HASH_LENGTH_TRIT;
    memcpy(chunk, &message[i], chunk_length * sizeof(trit_t));
    for (size_t j = 0; j < chunk_length; j++) {
      dest[i + j] = trit_sum(chunk[j], c->state[j]);
    }
    curl_absorb(c, chunk, chunk_length);
  }
}

void unmask(trit_t *const dest, trit_t const *const cipher, size_t const length, Curl *const c) {
  size_t chunk_length;
  for (size_t i = 0; i < length; i += HASH_LENGTH_TRIT) {
    chunk_length = length - i < HASH_LENGTH_TRIT ? length - i : HASH_LENGTH_TRIT;
    for (size_t j = 0; j < chunk_length; j++) {
      dest[i + j] = trit_sum(cipher[i + j], -c->state[j]);
    }
    curl_absorb(c, &dest[i], chunk_length);
  }
}
