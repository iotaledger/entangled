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

#include "common/defs.h"
#include "common/helpers/sign.h"
#include "common/kerl/kerl.h"
#include "common/sign/normalize.h"
#include "common/sign/v1/iss_kerl.h"
#include "common/trinary/trit_tryte.h"
#include "utils/export.h"

IOTA_EXPORT trit_t* iota_sign_address_gen_trits(trit_t const* const seed,
                                                size_t const index,
                                                size_t const security) {
  Kerl kerl;
  trit_t subseed[HASH_LENGTH_TRIT];
  size_t const key_length = security * ISS_KEY_LENGTH;
  trit_t* address = NULL;
  trit_t* key = NULL;

  if (!(security > 0 && security <= 3)) {
    return NULL;
  }

  if ((key = calloc(key_length, sizeof(trit_t))) == NULL) {
    return NULL;
  }

  if ((address = calloc(HASH_LENGTH_TRIT, sizeof(trit_t))) == NULL) {
    free(key);
    return NULL;
  }

  init_kerl(&kerl);

  memcpy(subseed, seed, HASH_LENGTH_TRIT);
  iss_kerl_subseed(subseed, subseed, index, &kerl);
  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH_TRIT);
  iss_kerl_key_digest(key, key, key_length, &kerl);
  iss_kerl_address(key, address, security * HASH_LENGTH_TRIT, &kerl);
  memset(key, 0, key_length * sizeof(trit_t));
  kerl_reset(&kerl);
  free(key);

  return address;
}

IOTA_EXPORT char* iota_sign_address_gen_trytes(char const* const seed,
                                               size_t const index,
                                               size_t const security) {
  trit_t seed_trits[HASH_LENGTH_TRIT];
  char* address = NULL;
  trit_t* address_trits = NULL;

  if (!(security > 0 && security <= 3)) {
    return NULL;
  }

  if ((address = calloc(HASH_LENGTH_TRYTE + 1, sizeof(tryte_t))) == NULL) {
    return NULL;
  }

  trytes_to_trits((tryte_t*)seed, seed_trits, HASH_LENGTH_TRYTE);
  if ((address_trits =
           iota_sign_address_gen_trits(seed_trits, index, security)) == NULL) {
    free(address);
    return NULL;
  }
  memset(seed_trits, 0, HASH_LENGTH_TRIT);
  trits_to_trytes(address_trits, (tryte_t*)address, HASH_LENGTH_TRIT);
  free(address_trits);

  return address;
}

IOTA_EXPORT flex_trit_t* iota_sign_address_gen_flex_trits(
    flex_trit_t const* const seed, size_t const index, size_t const security) {
  trit_t seed_trits[HASH_LENGTH_TRIT];
  flex_trit_t* address = NULL;
  trit_t* address_trits = NULL;

  if (!(security > 0 && security <= 3)) {
    return NULL;
  }

  if ((address = calloc(FLEX_TRIT_SIZE_243, sizeof(flex_trit_t))) == NULL) {
    return NULL;
  }

  flex_trits_to_trits(seed_trits, HASH_LENGTH_TRIT, seed, HASH_LENGTH_TRIT,
                      HASH_LENGTH_TRIT);
  if ((address_trits =
           iota_sign_address_gen_trits(seed_trits, index, security)) == NULL) {
    free(address);
    return NULL;
  }
  memset(seed_trits, 0, HASH_LENGTH_TRIT);
  flex_trits_from_trits(address, HASH_LENGTH_TRIT, address_trits,
                        HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  free(address_trits);

  return address;
}

IOTA_EXPORT char* iota_sign_signature_gen(char const* const seed,
                                          size_t const index,
                                          size_t const security,
                                          char const* const bundle_hash) {
  Kerl kerl;
  trit_t* key = NULL;
  tryte_t* signature = NULL;
  trit_t hash[HASH_LENGTH_TRIT];
  trit_t subseed[HASH_LENGTH_TRIT];
  const size_t key_length = security * ISS_KEY_LENGTH;

  key = calloc(key_length, sizeof(trit_t));
  if (!key) {
    return NULL;
  }
  signature = calloc(key_length / RADIX + 1, sizeof(tryte_t));
  if (!signature) {
    goto cleanup;
  }

  init_kerl(&kerl);

  trytes_to_trits((tryte_t*)seed, subseed, HASH_LENGTH_TRIT / RADIX);

  trytes_to_trits((tryte_t*)bundle_hash, hash, HASH_LENGTH_TRIT / RADIX);
  normalize_hash_to_trits(hash, hash);

  iss_kerl_subseed(subseed, subseed, index, &kerl);
  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH_TRIT * sizeof(trit_t));

  iss_kerl_signature(key, hash, key, key_length, &kerl);
  trits_to_trytes(key, (tryte_t*)signature, key_length);

  memset(key, 0, key_length * sizeof(trit_t));

cleanup:
  free(key);

  return (char*)signature;
}

IOTA_EXPORT flex_trit_t* iota_flex_sign_signature_gen(
    flex_trit_t const* const seed, size_t const index, size_t const security,
    flex_trit_t const* const bundle_hash) {
  Kerl kerl;
  trit_t* key = NULL;
  flex_trit_t* signature = NULL;
  trit_t hash[HASH_LENGTH_TRIT];
  trit_t subseed[HASH_LENGTH_TRIT];
  const size_t key_length = security * ISS_KEY_LENGTH;

  key = calloc(key_length, sizeof(trit_t));
  if (!key) {
    return NULL;
  }
  signature =
      calloc(NUM_FLEX_TRITS_FOR_TRITS(key_length) + 1, sizeof(flex_trit_t));
  if (!signature) {
    goto cleanup;
  }

  init_kerl(&kerl);

  flex_trits_to_trits(subseed, HASH_LENGTH_TRIT, seed, HASH_LENGTH_TRIT,
                      HASH_LENGTH_TRIT);
  normalize_flex_hash_to_trits(bundle_hash, hash);

  iss_kerl_subseed(subseed, subseed, index, &kerl);
  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH_TRIT * sizeof(trit_t));

  iss_kerl_signature(key, hash, key, key_length, &kerl);
  flex_trits_from_trits(signature, key_length, key, key_length, key_length);

  memset(key, 0, key_length * sizeof(trit_t));

cleanup:
  free(key);

  return signature;
}
