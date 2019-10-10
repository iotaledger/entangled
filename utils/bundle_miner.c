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
#include "common/trinary/add.h"
#include "common/trinary/trit_long.h"
#include "utils/bundle_miner.h"
#include "utils/macros.h"

#define OBSOLETE_TAG_OFFSET (243 + 81)
#define OBSOLETE_TAG_LENGTH 81

/**
 * Public functions
 */

double bundle_miner_probability_of_losing(byte_t const *const nb, uint64_t const security) {
  double probability = 0.0;
  double pi = 0.0;

  for (size_t i = 0; i < security * NORMALIZED_FRAGMENT_LENGTH; i++) {
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

void bundle_miner_normalized_bundle_max(byte_t const *const lhs, byte_t const *const rhs, byte_t *const max,
                                        size_t const length) {
  for (size_t i = 0; i < length; i++) {
    max[i] = MAX(lhs[i], rhs[i]);
  }
}

uint64_t bundle_miner_mine(byte_t const *const min, size_t const security, trit_t *const essence,
                           size_t const essence_length, uint32_t const count) {
  uint64_t index = 0;
  uint64_t best_index = 0;
  double best = 1.0;
  Kerl kerl;
  trit_t bundle_hash[HASH_LENGTH_TRIT];
  byte_t bundle_hash_normalized[NORMALIZED_BUNDLE_LENGTH];
  byte_t bundle_hash_normalized_min[NORMALIZED_BUNDLE_LENGTH];
  double p = 0.0;

  kerl_init(&kerl);
  index = trits_to_long(essence + OBSOLETE_TAG_OFFSET, OBSOLETE_TAG_LENGTH);

  for (size_t i = 0; i < count; i++) {
    kerl_reset(&kerl);
    kerl_absorb(&kerl, essence, essence_length);
    kerl_squeeze(&kerl, bundle_hash, HASH_LENGTH_TRIT);

    normalize_hash(bundle_hash, bundle_hash_normalized);

    if (normalized_hash_is_secure(bundle_hash_normalized, NORMALIZED_BUNDLE_LENGTH)) {
      bundle_miner_normalized_bundle_max(min, bundle_hash_normalized, bundle_hash_normalized_min,
                                         NORMALIZED_BUNDLE_LENGTH);
      p = bundle_miner_probability_of_losing(bundle_hash_normalized_min, security);
      if (p < best) {
        best = p;
        best_index = index;
      }
    }

    index += 1;
    add_assign(essence + OBSOLETE_TAG_OFFSET, OBSOLETE_TAG_LENGTH, 1);
  }

  return best_index;
}
