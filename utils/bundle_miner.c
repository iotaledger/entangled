/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <math.h>
#include <stdbool.h>

#include "common/crypto/iss/normalize.h"
#include "common/crypto/kerl/kerl.h"
#include "common/defs.h"
#include "common/trinary/trit_long.h"
#include "utils/bundle_miner.h"

#define OBSOLETE_TAG_OFFSET (243 + 81)
#define OBSOLETE_TAG_LENGTH 81

/**
 * Public functions
 */

double bundle_miner_probability_of_losing(byte_t const *const nb, uint64_t const n) {
  double probability = 0.0;
  double pi = 0.0;

  for (size_t i = 0; i < n * NORMALIZED_FRAGMENT_LENGTH; i++) {
    pi = 1.0 - ((double)(TRYTE_VALUE_MAX - nb[i]) / (double)(TRYTE_VALUE_MAX - TRYTE_VALUE_MIN));
    if (pi > 0.0) {
      if (probability == 0.0) {
        probability = 1.0;
      }
      probability *= pi;
    }
  }

  return probability;
}

double bundle_miner_security_level(double const probability, double const radix) {
  return log(1.0 / probability) / log(radix);
}

void bundle_miner_min_normalized_bundle(byte_t const *const a, byte_t const *const b, byte_t *const out,
                                        size_t const length) {
  for (size_t i = 0; i < length; i++) {
    if (TRYTE_VALUE_MAX - a[i] < TRYTE_VALUE_MAX - b[i]) {
      out[i] = a[i];
    } else {
      out[i] = b[i];
    }
  }
}

uint32_t bundle_miner_mine(byte_t const *const min, size_t const number_of_fragments, trit_t *const essence,
                           size_t const essence_length, uint32_t const offset, uint32_t const count) {
  uint32_t index = offset;
  uint32_t best_index = 0;
  double best = 1.0;
  Kerl kerl;
  trit_t bundle_hash[HASH_LENGTH_TRIT];
  byte_t bundle_hash_normalized[NORMALIZED_BUNDLE_LENGTH];
  byte_t bundle_hash_normalized_min[NORMALIZED_BUNDLE_LENGTH];
  bool secure = true;
  double p = 0.0;

  kerl_init(&kerl);

  while (index < offset + count) {
    long_to_trits(index, essence + OBSOLETE_TAG_OFFSET);
    kerl_reset(&kerl);

    kerl_absorb(&kerl, essence, essence_length);
    kerl_squeeze(&kerl, bundle_hash, HASH_LENGTH_TRIT);

    memset(bundle_hash_normalized, TRYTE_VALUE_MIN, NORMALIZED_BUNDLE_LENGTH);

    normalize_hash(bundle_hash, bundle_hash_normalized);

    secure = true;
    for (size_t i = 0; i < NORMALIZED_BUNDLE_LENGTH; i++) {
      if (bundle_hash_normalized[i] == TRYTE_VALUE_MAX) {
        secure = false;
        break;
      }
    }

    if (secure) {
      memset(bundle_hash_normalized_min, TRYTE_VALUE_MIN, NORMALIZED_BUNDLE_LENGTH);
      bundle_miner_min_normalized_bundle(min, bundle_hash_normalized, bundle_hash_normalized_min,
                                         NORMALIZED_BUNDLE_LENGTH);

      p = bundle_miner_probability_of_losing(bundle_hash_normalized_min, number_of_fragments);
      if (p < best) {
        best = p;
        best_index = index;
      }
    }

    index += 1;
  }

  return best_index;
}
