#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/helpers/sign.h"
#include "common/kerl/kerl.h"
#include "common/sign/v1/iss_kerl.h"
#include "common/trinary/trit_tryte.h"
#include "utils/export.h"

IOTA_EXPORT char* iota_sign_address_gen(char const* const seed,
                                        size_t const index,
                                        size_t const security) {
  if (!(security > 0 && security <= 3)) {
    return NULL;
  }

  Kerl kerl;
  trit_t* key = NULL;
  char* address = NULL;
  trit_t subseed[HASH_LENGTH];
  const size_t key_length = security * ISS_KEY_LENGTH;

  key = calloc(sizeof(trit_t) * key_length, sizeof(trit_t));
  if (!key) {
    return NULL;
  }
  address =
      calloc(sizeof(tryte_t) * (HASH_LENGTH / RADIX + 1), sizeof(tryte_t));
  if (!address) {
    goto cleanup;
  }
  init_kerl(&kerl);

  trytes_to_trits((tryte_t*)seed, subseed, HASH_LENGTH / RADIX);
  iss_kerl_subseed(subseed, subseed, index, &kerl);

  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  iss_kerl_key_digest(key, key, key_length, &kerl);
  iss_kerl_address(key, key, security * HASH_LENGTH, &kerl);

  kerl_reset(&kerl);

  trits_to_trytes(key, (tryte_t*)address, HASH_LENGTH);

cleanup:
  memset(key, 0, key_length * sizeof(trit_t));
  free(key);

  return address;
}

IOTA_EXPORT char* iota_sign_signature_gen(char const* const seed,
                                          size_t const index,
                                          size_t const security,
                                          char const* const bundle_hash) {
  Kerl kerl;
  trit_t* key = NULL;
  tryte_t* signature = NULL;
  trit_t hash[HASH_LENGTH];
  trit_t subseed[HASH_LENGTH];
  const size_t key_length = security * ISS_KEY_LENGTH;

  key = calloc(sizeof(trit_t) * key_length, sizeof(trit_t));
  if (!key) {
    return NULL;
  }
  signature =
      calloc(sizeof(tryte_t) * (key_length / RADIX + 1), sizeof(tryte_t));
  if (!signature) {
    goto cleanup;
  }

  init_kerl(&kerl);

  trytes_to_trits((tryte_t*)seed, subseed, HASH_LENGTH / RADIX);
  trytes_to_trits((tryte_t*)bundle_hash, hash, HASH_LENGTH / RADIX);
  iss_kerl_subseed(subseed, subseed, index, &kerl);
  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  iss_kerl_signature(key, hash, key, key_length, &kerl);
  trits_to_trytes(key, (tryte_t*)signature, key_length);

cleanup:
  memset(key, 0, key_length * sizeof(trit_t));
  free(key);

  return (char*)signature;
}

IOTA_EXPORT flex_trit_t* iota_flex_sign_address_gen(
    flex_trit_t const* const seed, size_t const index, size_t const security) {
  if (!(security > 0 && security <= 3)) {
    return NULL;
  }

  Kerl kerl;
  trit_t* key = NULL;
  flex_trit_t* address = NULL;
  trit_t subseed[HASH_LENGTH];
  const size_t key_length = security * ISS_KEY_LENGTH;

  key = calloc(sizeof(trit_t) * key_length, sizeof(trit_t));
  if (!key) {
    return NULL;
  }
  address =
      calloc(sizeof(flex_trit_t) * (num_flex_trits_for_trits(HASH_LENGTH) + 1),
             sizeof(flex_trit_t));
  if (!address) {
    goto cleanup;
  }

  init_kerl(&kerl);

  flex_trits_to_trits(subseed, HASH_LENGTH, seed, HASH_LENGTH, HASH_LENGTH);
  iss_kerl_subseed(subseed, subseed, index, &kerl);

  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  iss_kerl_key_digest(key, key, key_length, &kerl);
  iss_kerl_address(key, key, security * HASH_LENGTH, &kerl);

  kerl_reset(&kerl);

  flex_trits_from_trits(address, HASH_LENGTH, key, key_length, HASH_LENGTH);

cleanup:
  memset(key, 0, key_length * sizeof(trit_t));
  free(key);

  return address;
}

IOTA_EXPORT flex_trit_t* iota_flex_sign_signature_gen(
    flex_trit_t const* const seed, size_t const index, size_t const security,
    flex_trit_t const* const bundle_hash) {
  Kerl kerl;
  trit_t* key = NULL;
  flex_trit_t* signature = NULL;
  trit_t hash[HASH_LENGTH];
  trit_t subseed[HASH_LENGTH];
  const size_t key_length = security * ISS_KEY_LENGTH;

  key = calloc(sizeof(trit_t) * key_length, sizeof(trit_t));
  if (!key) {
    return NULL;
  }
  signature =
      calloc(sizeof(flex_trit_t) * (num_flex_trits_for_trits(key_length) + 1),
             sizeof(flex_trit_t));
  if (!signature) {
    goto cleanup;
  }

  init_kerl(&kerl);

  flex_trits_to_trits(subseed, HASH_LENGTH, seed, HASH_LENGTH, HASH_LENGTH);
  flex_trits_to_trits(hash, HASH_LENGTH, bundle_hash, HASH_LENGTH, HASH_LENGTH);

  iss_kerl_subseed(subseed, subseed, index, &kerl);
  iss_kerl_key(subseed, key, key_length, &kerl);
  memset(subseed, 0, HASH_LENGTH * sizeof(trit_t));

  iss_kerl_signature(key, hash, key, key_length, &kerl);
  flex_trits_from_trits(signature, key_length, key, key_length, key_length);

cleanup:
  memset(key, 0, key_length * sizeof(trit_t));
  free(key);

  return signature;
}

#undef RADIX
#undef NONCE_LENGTH
