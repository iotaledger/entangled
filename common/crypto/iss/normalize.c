/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/crypto/iss/normalize.h"
#include "common/defs.h"
#include "common/trinary/trit_long.h"

void normalize_hash(trit_t const *const hash, byte_t *const normalized_hash) {
  for (int i = 0; i < SECURITY_LEVEL_MAX; i++) {
    int sum = 0;
    for (int j = i * NORMALIZED_FRAGMENT_LENGTH; j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
      normalized_hash[j] = (hash[j * TRYTE_WIDTH] + hash[j * TRYTE_WIDTH + 1] * 3 + hash[j * TRYTE_WIDTH + 2] * 9);
      sum += normalized_hash[j];
    }
    if (sum > 0) {
      while (sum-- > 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH; j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_hash[j] > TRYTE_VALUE_MIN) {
            normalized_hash[j]--;
            break;
          }
        }
      }
    } else {
      while (sum++ < 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH; j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_hash[j] < TRYTE_VALUE_MAX) {
            normalized_hash[j]++;
            break;
          }
        }
      }
    }
  }
}

void normalize_hash_to_trits(trit_t const *const hash, trit_t *const normalized_hash) {
  byte_t normalized_bundle_bytes[HASH_LENGTH_TRYTE];

  normalize_hash(hash, normalized_bundle_bytes);
  for (int c = 0; c < HASH_LENGTH_TRYTE; ++c) {
    long_to_trits(normalized_bundle_bytes[c], &normalized_hash[c * RADIX]);
  }
}

void normalize_flex_hash(flex_trit_t const *const hash, byte_t *const normalized_hash) {
  trit_t hash_trits[HASH_LENGTH_TRIT];
  flex_trits_to_trits(hash_trits, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

  normalize_hash(hash_trits, normalized_hash);
}

void normalize_flex_hash_to_trits(flex_trit_t const *const hash, trit_t *const normalized_hash) {
  byte_t normalized_bundle_bytes[HASH_LENGTH_TRYTE];

  normalize_flex_hash(hash, normalized_bundle_bytes);
  for (int c = 0; c < HASH_LENGTH_TRYTE; ++c) {
    long_to_trits(normalized_bundle_bytes[c], &normalized_hash[c * RADIX]);
  }
}
