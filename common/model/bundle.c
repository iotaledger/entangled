/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/bundle.h"

#define MAX_TRYTE_VALUE 13
#define MIN_TRYTE_VALUE -13
#define TRYTE_WIDTH 3
#define NUMBER_OF_SECURITY_LEVELS 3
#define NORMALIZED_FRAGMENT_LENGTH 27

void normalized_bundle(trit_t const *const bundle,
                       trit_t *const normalized_bundle) {
  for (int i = 0; i < NUMBER_OF_SECURITY_LEVELS; i++) {
    int sum = 0;
    for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
         j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
      normalized_bundle[j] =
          (bundle[j * TRYTE_WIDTH] + bundle[j * TRYTE_WIDTH + 1] * 3 +
           bundle[j * TRYTE_WIDTH + 2] * 9);
      sum += normalized_bundle[j];
    }
    if (sum > 0) {
      while (sum-- > 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
             j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_bundle[j] > MIN_TRYTE_VALUE) {
            normalized_bundle[j]--;
            break;
          }
        }
      }
    } else {
      while (sum++ < 0) {
        for (int j = i * NORMALIZED_FRAGMENT_LENGTH;
             j < (i + 1) * NORMALIZED_FRAGMENT_LENGTH; j++) {
          if (normalized_bundle[j] < MAX_TRYTE_VALUE) {
            normalized_bundle[j]++;
            break;
          }
        }
      }
    }
  }
}
