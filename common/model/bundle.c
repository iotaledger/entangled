/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/bundle.h"

#define TRYTE_HASH_LENGTH 81
#define TRIT_HASH_LENGTH 243
#define MAX_TRYTE_VALUE 13
#define MIN_TRYTE_VALUE -13
#define TRYTE_WIDTH 3
#define NUMBER_OF_SECURITY_LEVELS 3
#define NORMALIZED_FRAGMENT_LENGTH 27

void normalized_bundle(trit_t const *const bundle_hash,
                       byte_t *const normalized_bundle_hash) {
  for (int i = 0; i < NUMBER_OF_SECURITY_LEVELS; i++) {
    int sum = 0;
    for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
         j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
      normalized_bundle_hash[j] =
          (bundle_hash[j * TRYTE_WIDTH] + bundle_hash[j * TRYTE_WIDTH + 1] * 3 +
           bundle_hash[j * TRYTE_WIDTH + 2] * 9);
      sum += normalized_bundle_hash[j];
    }
    if (sum > 0) {
      while (sum-- > 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
             j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_bundle_hash[j] > MIN_TRYTE_VALUE) {
            normalized_bundle_hash[j]--;
            break;
          }
        }
      }
    } else {
      while (sum++ < 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
             j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_bundle_hash[j] < MAX_TRYTE_VALUE) {
            normalized_bundle_hash[j]++;
            break;
          }
        }
      }
    }
  }
}

void flex_normalized_bundle(flex_trit_t const *const bundle_hash,
                            byte_t *const normalized_bundle_hash) {
  trit_t bundle_hash_trits[TRIT_HASH_LENGTH];

  flex_trits_to_trits(bundle_hash_trits, TRIT_HASH_LENGTH, bundle_hash,
                      TRIT_HASH_LENGTH, TRIT_HASH_LENGTH);
  normalized_bundle(bundle_hash_trits, normalized_bundle_hash);
}
