/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <math.h>

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

double bundle_miner_probability_of_losing(byte_t const *const normalized_hash, uint8_t const security) {
  double p = 0.0;
  double pi = 0.0;

  for (size_t i = 0; i < security * NORMALIZED_FRAGMENT_LENGTH; i++) {
    pi = 1.0 - ((double)(TRYTE_VALUE_MAX - normalized_hash[i]) / (double)(TRYTE_VALUE_MAX - TRYTE_VALUE_MIN));
    if (pi > 0.0) {
      if (p == 0.0) {
        p = 1.0;
      }
      p *= pi;
    }
  }

  return p;
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

uint64_t bundle_miner_mine(byte_t const *const bundle_normalized_max, uint8_t const security, trit_t *const essence,
                           size_t const essence_length, uint32_t const count) {
  Kerl kerl;
  uint64_t index = 0;
  uint64_t optimal_index = 0;
  double probability = 0.0;
  double optimal_probability = 1.0;
  trit_t candidate[HASH_LENGTH_TRIT];
  byte_t candidate_normalized[NORMALIZED_BUNDLE_LENGTH];
  byte_t candidate_normalized_max[NORMALIZED_BUNDLE_LENGTH];

  kerl_init(&kerl);
  index = trits_to_long(essence + OBSOLETE_TAG_OFFSET, OBSOLETE_TAG_LENGTH);

  for (size_t i = 0; i < count; i++) {
    kerl_reset(&kerl);
    kerl_absorb(&kerl, essence, essence_length);
    kerl_squeeze(&kerl, candidate, HASH_LENGTH_TRIT);

    normalize_hash(candidate, candidate_normalized);

    if (normalized_hash_is_secure(candidate_normalized, NORMALIZED_BUNDLE_LENGTH)) {
      bundle_miner_normalized_bundle_max(bundle_normalized_max, candidate_normalized, candidate_normalized_max,
                                         NORMALIZED_BUNDLE_LENGTH);
      probability = bundle_miner_probability_of_losing(candidate_normalized_max, security);
      if (probability < optimal_probability) {
        optimal_probability = probability;
        optimal_index = index;
      }
    }

    index += 1;
    add_assign(essence + OBSOLETE_TAG_OFFSET, OBSOLETE_TAG_LENGTH, 1);
  }

  return optimal_index;
}
