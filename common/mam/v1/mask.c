#include <string.h>

#include "common/mam/v1/mask.h"
#include "common/trinary/add.h"

int mask(trit_t *dest, trit_t *message, size_t l, Curl *const c) {
  size_t chunk_length;
  trit_t chunk[HASH_LENGTH];
  for (size_t i = 0; i < l; i += HASH_LENGTH) {
    chunk_length = l - i < HASH_LENGTH ? l - i : HASH_LENGTH;
    memcpy(chunk, &message[i], chunk_length * sizeof(trit_t));
    for (size_t j = 0; j < chunk_length; j++) {
      dest[i + j] = trit_sum(chunk[j], c->state[j]);
    }
    curl_absorb(c, chunk, chunk_length);
  }
  return 0;
}

int unmask(trit_t *dest, trit_t *cipher, size_t l, Curl *const c) {
  size_t chunk_length;
  for (size_t i = 0; i < l; i += HASH_LENGTH) {
    chunk_length = l - i < HASH_LENGTH ? l - i : HASH_LENGTH;
    for (size_t j = 0; j < chunk_length; j++) {
      dest[i + j] = trit_sum(cipher[i + j], -c->state[j]);
    }
    curl_absorb(c, &dest[i], chunk_length);
  }
  return 0;
}
