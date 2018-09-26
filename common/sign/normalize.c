/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/sign/normalize.h"

void normalize_hash(flex_trit_t const *const hash,
                    byte_t *const normalized_hash) {
  trit_t hash_trits[NUM_TRITS_HASH];
  flex_trits_to_trits(hash_trits, NUM_TRITS_HASH, hash, NUM_TRITS_HASH,
                      NUM_TRITS_HASH);

  for (int i = 0; i < NUMBER_OF_SECURITY_LEVELS; i++) {
    int sum = 0;
    for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
         j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
      normalized_hash[j] =
          (hash_trits[j * TRYTE_WIDTH] + hash_trits[j * TRYTE_WIDTH + 1] * 3 +
           hash_trits[j * TRYTE_WIDTH + 2] * 9);
      sum += normalized_hash[j];
    }
    if (sum > 0) {
      while (sum-- > 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
             j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_hash[j] > MIN_TRYTE_VALUE) {
            normalized_hash[j]--;
            break;
          }
        }
      }
    } else {
      while (sum++ < 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
             j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_hash[j] < MAX_TRYTE_VALUE) {
            normalized_hash[j]++;
            break;
          }
        }
      }
    }
  }
}
