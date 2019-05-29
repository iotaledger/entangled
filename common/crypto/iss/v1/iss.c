/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/crypto/curl-p/trit.h"
#include "common/crypto/iss/v1/iss.h"
#include "common/trinary/add.h"

#define HASH_TRYTE_VAL(hash, i) (hash[i * TRYTE_WIDTH] + hash[i * TRYTE_WIDTH + 1] * 3 + hash[i * TRYTE_WIDTH + 2] * 9)

int iss_subseed(sponge_t *const sponge, trit_t const *const seed, trit_t *const out, int64_t const index) {
  memcpy(out, seed, HASH_LENGTH_TRIT * sizeof(trit_t));
  add_assign(out, HASH_LENGTH_TRIT, index);
  sponge_absorb(sponge, out, HASH_LENGTH_TRIT);
  sponge_squeeze(sponge, out, HASH_LENGTH_TRIT);
  sponge_reset(sponge);

  return 0;
}

int iss_key(sponge_t *const sponge, trit_t const *const subseed, trit_t *key, size_t const key_length) {
  if (key_length % HASH_LENGTH_TRIT) {
    return -1;
  }

  trit_t *const end = &key[key_length];

  sponge_absorb(sponge, subseed, HASH_LENGTH_TRIT);

  for (; key < end; key = &key[HASH_LENGTH_TRIT]) {
    sponge_squeeze(sponge, key, HASH_LENGTH_TRIT);
  }

  sponge_reset(sponge);

  return 0;
}

int iss_key_digest(sponge_t *const sponge, trit_t *key, trit_t *digest, size_t const key_length) {
  if (key_length % ISS_KEY_LENGTH) {
    return -1;
  }

  size_t i;
  trit_t *const k_start = key;
  trit_t *const k_end = &key[key_length];
  trit_t *const d_end = &digest[HASH_LENGTH_TRIT * (key_length / ISS_KEY_LENGTH)];

  for (; key < k_end; key = &key[HASH_LENGTH_TRIT]) {
    for (i = 0; i < 26; i++) {
      sponge_absorb(sponge, key, HASH_LENGTH_TRIT);
      sponge_squeeze(sponge, key, HASH_LENGTH_TRIT);
      sponge_reset(sponge);
    }
  }

  key = k_start;

  for (; digest < d_end; digest = &digest[HASH_LENGTH_TRIT]) {
    sponge_absorb(sponge, key, ISS_KEY_LENGTH);
    sponge_squeeze(sponge, digest, HASH_LENGTH_TRIT);
    sponge_reset(sponge);
    key = &key[ISS_KEY_LENGTH];
  }

  return 0;
}

int iss_address(sponge_t *const sponge, trit_t const *const digest, trit_t *const address, size_t const digest_length) {
  sponge_absorb(sponge, digest, digest_length);
  sponge_squeeze(sponge, address, HASH_LENGTH_TRIT);
  sponge_reset(sponge);

  return 0;
}

int iss_signature(sponge_t *const sponge, trit_t *sig, trit_t const *const hash, trit_t const *const key,
                  size_t key_len) {
  trit_t *se = &sig[key_len];

  if (sig != key) {
    memcpy(sig, key, key_len * sizeof(trit_t));
  }

  for (size_t i = 0; sig < se; i++, sig = &sig[HASH_LENGTH_TRIT]) {
    for (size_t j = 0; j < (size_t)(TRYTE_VALUE_MAX - HASH_TRYTE_VAL(hash, i)); j++) {
      sponge_absorb(sponge, sig, HASH_LENGTH_TRIT);
      sponge_squeeze(sponge, sig, HASH_LENGTH_TRIT);
      sponge_reset(sponge);
    }
  }

  return 0;
}

int iss_sig_digest(sponge_t *const sponge, trit_t *const dig, trit_t const *const hash, trit_t *sig,
                   size_t const sig_len) {
  trit_t *sig_start = sig, *sig_end = &sig[sig_len];

  for (size_t i = 0; sig < sig_end; i++, sig = &sig[HASH_LENGTH_TRIT]) {
    for (size_t j = 0; j < (size_t)(HASH_TRYTE_VAL(hash, i) - TRYTE_VALUE_MIN); j++) {
      sponge_absorb(sponge, sig, HASH_LENGTH_TRIT);
      sponge_squeeze(sponge, sig, HASH_LENGTH_TRIT);
      sponge_reset(sponge);
    }
  }

  sponge_absorb(sponge, sig_start, sig_len);
  sponge_squeeze(sponge, dig, HASH_LENGTH_TRIT);
  sponge_reset(sponge);

  return 0;
}

int iss_merkle_root(sponge_t *const sponge, trit_t *const hash, trit_t const *const siblings,
                    size_t const siblings_number, size_t const leaf_index) {
  for (size_t i = 0, j = 1; i < siblings_number; i++, j <<= 1) {
    // if index is a right node, absorb a sibling (left) then the hash
    if (leaf_index & j) {
      sponge_absorb(sponge, &siblings[i * HASH_LENGTH_TRIT], HASH_LENGTH_TRIT);
      sponge_absorb(sponge, hash, HASH_LENGTH_TRIT);
    }
    // if index is a left node, absorb the hash then a sibling (right)
    else {
      sponge_absorb(sponge, hash, HASH_LENGTH_TRIT);
      sponge_absorb(sponge, &siblings[i * HASH_LENGTH_TRIT], HASH_LENGTH_TRIT);
    }
    sponge_squeeze(sponge, hash, HASH_LENGTH_TRIT);
    sponge_reset(sponge);
  }

  return 0;
}
