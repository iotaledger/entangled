/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/crypto/iss/normalize.h"
#include "common/crypto/iss/v1/iss_kerl.h"
#include "common/crypto/kerl/kerl.h"
#include "common/defs.h"
#include "common/helpers/sign.h"
#include "common/trinary/trit_tryte.h"
#include "utils/export.h"

IOTA_EXPORT trit_t* iota_sign_address_gen_trits(trit_t const* const seed, size_t const index, size_t const security) {
  Kerl kerl;
  trit_t subseed[HASH_LENGTH_TRIT];
  size_t const key_length = security * ISS_KEY_LENGTH;
  trit_t* address = NULL;
  trit_t* key = NULL;

  if (seed == NULL || !(security > 0 && security <= 3)) {
    return NULL;
  }

  if ((key = (trit_t*)calloc(key_length, sizeof(trit_t))) == NULL) {
    return NULL;
  }

  if ((address = (trit_t*)calloc(HASH_LENGTH_TRIT, sizeof(trit_t))) == NULL) {
    memset_safe(key, key_length * sizeof(trit_t), 0, key_length * sizeof(trit_t));
    free(key);
    return NULL;
  }

  kerl_init(&kerl);
  memcpy(subseed, seed, HASH_LENGTH_TRIT);
  iss_kerl_subseed(subseed, subseed, index, &kerl);
  iss_kerl_key(subseed, key, key_length, &kerl);
  memset_safe(subseed, HASH_LENGTH_TRIT, 0, HASH_LENGTH_TRIT);
  iss_kerl_key_digest(key, key, key_length, &kerl);
  iss_kerl_address(key, address, security * HASH_LENGTH_TRIT, &kerl);
  memset_safe(key, key_length * sizeof(trit_t), 0, key_length * sizeof(trit_t));
  kerl_reset(&kerl);
  free(key);

  return address;
}

IOTA_EXPORT char* iota_sign_address_gen_trytes(char const* const seed, size_t const index, size_t const security) {
  trit_t seed_trits[HASH_LENGTH_TRIT];
  char* address = NULL;
  trit_t* address_trits = NULL;

  if (seed == NULL || !(security > 0 && security <= 3)) {
    return NULL;
  }

  if ((address = (char*)calloc(HASH_LENGTH_TRYTE + 1, sizeof(tryte_t))) == NULL) {
    return NULL;
  }

  trytes_to_trits((tryte_t*)seed, seed_trits, HASH_LENGTH_TRYTE);
  if ((address_trits = iota_sign_address_gen_trits(seed_trits, index, security)) == NULL) {
    memset_safe(seed_trits, HASH_LENGTH_TRIT, 0, HASH_LENGTH_TRIT);
    free(address);
    return NULL;
  }
  memset_safe(seed_trits, HASH_LENGTH_TRIT, 0, HASH_LENGTH_TRIT);
  trits_to_trytes(address_trits, (tryte_t*)address, HASH_LENGTH_TRIT);
  free(address_trits);

  return address;
}

IOTA_EXPORT flex_trit_t* iota_sign_address_gen_flex_trits(flex_trit_t const* const seed, size_t const index,
                                                          size_t const security) {
  trit_t seed_trits[HASH_LENGTH_TRIT];
  flex_trit_t* address = NULL;
  trit_t* address_trits = NULL;

  if (seed == NULL || !(security > 0 && security <= 3)) {
    return NULL;
  }

  if ((address = (flex_trit_t*)calloc(FLEX_TRIT_SIZE_243, sizeof(flex_trit_t))) == NULL) {
    return NULL;
  }

  flex_trits_to_trits(seed_trits, HASH_LENGTH_TRIT, seed, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  if ((address_trits = iota_sign_address_gen_trits(seed_trits, index, security)) == NULL) {
    memset_safe(seed_trits, HASH_LENGTH_TRIT, 0, HASH_LENGTH_TRIT);
    free(address);
    return NULL;
  }
  memset_safe(seed_trits, HASH_LENGTH_TRIT, 0, HASH_LENGTH_TRIT);
  flex_trits_from_trits(address, HASH_LENGTH_TRIT, address_trits, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  free(address_trits);

  return address;
}

IOTA_EXPORT trit_t* iota_sign_signature_gen_trits(trit_t const* const seed, size_t const index, size_t const security,
                                                  trit_t const* const bundle_hash) {
  Kerl kerl;
  trit_t subseed[HASH_LENGTH_TRIT];
  trit_t normalized_bundle_hash[HASH_LENGTH_TRIT];
  size_t const key_length = security * ISS_KEY_LENGTH;
  trit_t* key = NULL;

  if (seed == NULL || !(security > 0 && security <= 3) || bundle_hash == NULL) {
    return NULL;
  }

  if ((key = (trit_t*)calloc(key_length, sizeof(trit_t))) == NULL) {
    return NULL;
  }

  kerl_init(&kerl);
  memcpy(subseed, seed, HASH_LENGTH_TRIT);
  iss_kerl_subseed(subseed, subseed, index, &kerl);
  iss_kerl_key(subseed, key, key_length, &kerl);
  memset_safe(subseed, HASH_LENGTH_TRIT * sizeof(trit_t), 0, HASH_LENGTH_TRIT * sizeof(trit_t));
  normalize_hash_to_trits(bundle_hash, normalized_bundle_hash);
  iss_kerl_signature(key, normalized_bundle_hash, key, key_length, &kerl);
  kerl_reset(&kerl);

  return key;
}

IOTA_EXPORT char* iota_sign_signature_gen_trytes(char const* const seed, size_t const index, size_t const security,
                                                 char const* const bundle_hash) {
  trit_t seed_trits[HASH_LENGTH_TRIT];
  trit_t bundle_hash_trits[HASH_LENGTH_TRIT];
  size_t const signature_length = security * ISS_KEY_LENGTH;
  tryte_t* signature = NULL;
  trit_t* signature_trits = NULL;

  if (seed == NULL || !(security > 0 && security <= 3) || bundle_hash == NULL) {
    return NULL;
  }

  if ((signature = (tryte_t*)calloc(signature_length / RADIX + 1, sizeof(tryte_t))) == NULL) {
    return NULL;
  }

  trytes_to_trits((tryte_t*)seed, seed_trits, HASH_LENGTH_TRYTE);
  trytes_to_trits((tryte_t*)bundle_hash, bundle_hash_trits, HASH_LENGTH_TRYTE);
  if ((signature_trits = iota_sign_signature_gen_trits(seed_trits, index, security, bundle_hash_trits)) == NULL) {
    memset_safe(seed_trits, HASH_LENGTH_TRIT, 0, HASH_LENGTH_TRIT);
    free(signature);
    return NULL;
  }
  memset_safe(seed_trits, HASH_LENGTH_TRIT, 0, HASH_LENGTH_TRIT);
  trits_to_trytes(signature_trits, (tryte_t*)signature, signature_length);
  free(signature_trits);

  return (char*)signature;
}

IOTA_EXPORT flex_trit_t* iota_sign_signature_gen_flex_trits(flex_trit_t const* const seed, size_t const index,
                                                            size_t const security,
                                                            flex_trit_t const* const bundle_hash) {
  trit_t seed_trits[HASH_LENGTH_TRIT];
  trit_t bundle_hash_trits[HASH_LENGTH_TRIT];
  size_t const signature_length = security * ISS_KEY_LENGTH;
  flex_trit_t* signature = NULL;
  trit_t* signature_trits = NULL;

  if (seed == NULL || !(security > 0 && security <= 3) || bundle_hash == NULL) {
    return NULL;
  }

  if ((signature = (flex_trit_t*)calloc(NUM_FLEX_TRITS_FOR_TRITS(signature_length), sizeof(flex_trit_t))) == NULL) {
    return NULL;
  }

  flex_trits_to_trits(seed_trits, HASH_LENGTH_TRIT, seed, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  flex_trits_to_trits(bundle_hash_trits, HASH_LENGTH_TRIT, bundle_hash, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  if ((signature_trits = iota_sign_signature_gen_trits(seed_trits, index, security, bundle_hash_trits)) == NULL) {
    memset_safe(seed_trits, HASH_LENGTH_TRIT, 0, HASH_LENGTH_TRIT);
    free(signature);
    return NULL;
  }
  memset_safe(seed_trits, HASH_LENGTH_TRIT, 0, HASH_LENGTH_TRIT);
  flex_trits_from_trits(signature, signature_length, signature_trits, signature_length, signature_length);
  free(signature_trits);

  return signature;
}
