/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/defs.h"
#include "common/trinary/add.h"

#define CAT(A, ...) _CAT(A, __VA_ARGS__)
#define _CAT(A, ...) A##__VA_ARGS__

#define _ISS_PREFIX(A) CAT(CAT(CAT(iss_, HASH_PREFIX), _), A)
#define _HASH_PREFIX(A) CAT(CAT(HASH_PREFIX, _), A)

int _ISS_PREFIX(subseed)(trit_t const *const seed, trit_t *const out,
                         int64_t const index, HASH_STATE *const state) {
  memcpy(out, seed, HASH_LENGTH_TRIT * sizeof(trit_t));
  add_assign(out, HASH_LENGTH_TRIT, index);
  _HASH_PREFIX(absorb)(state, out, HASH_LENGTH_TRIT);
  _HASH_PREFIX(squeeze)(state, out, HASH_LENGTH_TRIT);
  _HASH_PREFIX(reset)(state);
  return 0;
}

int _ISS_PREFIX(key)(trit_t const *const subseed, trit_t *key,
                     size_t const key_length, HASH_STATE *const state) {
  if (key_length % HASH_LENGTH_TRIT) return -1;
  trit_t *const end = &key[key_length];

  _HASH_PREFIX(absorb)(state, subseed, HASH_LENGTH_TRIT);

  for (; key < end; key = &key[HASH_LENGTH_TRIT]) {
    _HASH_PREFIX(squeeze)(state, key, HASH_LENGTH_TRIT);
  }

  _HASH_PREFIX(reset)(state);

  return 0;
}

int _ISS_PREFIX(key_digest)(trit_t *key, trit_t *digest,
                            size_t const key_length, HASH_STATE *const state) {
  if (key_length % ISS_KEY_LENGTH) return -1;

  size_t i;
  trit_t *const k_start = key;
  trit_t *const k_end = &key[key_length];
  trit_t *const d_end =
      &digest[HASH_LENGTH_TRIT * (key_length / ISS_KEY_LENGTH)];

  for (; key < k_end; key = &key[HASH_LENGTH_TRIT]) {
    for (i = 0; i < 26; i++) {
      _HASH_PREFIX(absorb)(state, key, HASH_LENGTH_TRIT);
      _HASH_PREFIX(squeeze)(state, key, HASH_LENGTH_TRIT);
      _HASH_PREFIX(reset)(state);
    }
  }

  key = k_start;

  for (; digest < d_end; digest = &digest[HASH_LENGTH_TRIT]) {
    _HASH_PREFIX(absorb)(state, key, ISS_KEY_LENGTH);
    _HASH_PREFIX(squeeze)(state, digest, HASH_LENGTH_TRIT);
    _HASH_PREFIX(reset)(state);

    key = &key[ISS_KEY_LENGTH];
  }
  return 0;
}

int _ISS_PREFIX(address)(trit_t const *const digest, trit_t *const address,
                         size_t const digest_length, HASH_STATE *const state) {
  _HASH_PREFIX(absorb)(state, digest, digest_length);
  _HASH_PREFIX(squeeze)(state, address, HASH_LENGTH_TRIT);
  _HASH_PREFIX(reset)(state);
  return 0;
}

#define HASH_TRYTE_VAL(hash, i)                            \
  (hash[i * TRYTE_WIDTH] + hash[i * TRYTE_WIDTH + 1] * 3 + \
   hash[i * TRYTE_WIDTH + 2] * 9)

int _ISS_PREFIX(signature)(trit_t *sig, trit_t const *const hash,
                           trit_t const *const key, size_t key_len,
                           HASH_STATE *const state) {
  size_t i, j;
  trit_t *se = &sig[key_len];
  if (sig != key) memcpy(sig, key, key_len * sizeof(trit_t));
  for (i = 0; sig < se; i++, sig = &sig[HASH_LENGTH_TRIT]) {
    for (j = 0; j < (TRYTE_VALUE_MAX - HASH_TRYTE_VAL(hash, i)); j++) {
      _HASH_PREFIX(absorb)(state, sig, HASH_LENGTH_TRIT);
      _HASH_PREFIX(squeeze)(state, sig, HASH_LENGTH_TRIT);
      _HASH_PREFIX(reset)(state);
    }
  }
  _HASH_PREFIX(reset)(state);
  return 0;
}

int _ISS_PREFIX(sig_digest)(trit_t *const dig, trit_t const *const hash,
                            trit_t *sig, size_t const sig_len,
                            HASH_STATE *const state) {
  size_t i, j;
  trit_t *sig_start = sig, *sig_end = &sig[sig_len];
  for (i = 0; sig < sig_end; i++, sig = &sig[HASH_LENGTH_TRIT]) {
    for (j = 0; j < (HASH_TRYTE_VAL(hash, i) - TRYTE_VALUE_MIN); j++) {
      _HASH_PREFIX(absorb)(state, sig, HASH_LENGTH_TRIT);
      _HASH_PREFIX(squeeze)(state, sig, HASH_LENGTH_TRIT);
      _HASH_PREFIX(reset)(state);
    }
  }
  _HASH_PREFIX(absorb)(state, sig_start, sig_len);
  _HASH_PREFIX(squeeze)(state, dig, HASH_LENGTH_TRIT);
  return 0;
}

#undef HASH_TRYTE_VAL
#undef _ISS_PREFIX
#undef _HASH_PREFIX

#undef CAT
#undef _CAT
