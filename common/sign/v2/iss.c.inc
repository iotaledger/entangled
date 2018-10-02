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
                         int64_t const index, HASH_STATE *const c) {
  memcpy(out, seed, HASH_LENGTH_TRIT * sizeof(trit_t));
  add_assign(out, HASH_LENGTH_TRIT, index);
  _HASH_PREFIX(absorb)(c, out, HASH_LENGTH_TRIT);
  _HASH_PREFIX(squeeze)(c, out, HASH_LENGTH_TRIT);
  _HASH_PREFIX(reset)(c);
  return 0;
}

int _ISS_PREFIX(key)(trit_t const *const subseed, trit_t *key,
                     size_t const key_length, HASH_STATE *const c) {
  if (key_length % HASH_LENGTH_TRIT) return -1;
  trit_t *const e = &key[key_length];

  _HASH_PREFIX(absorb)(c, subseed, HASH_LENGTH_TRIT);
  _HASH_PREFIX(squeeze)(c, key, key_length);
  _HASH_PREFIX(reset)(c);

  for (; key < e; key = &key[HASH_LENGTH_TRIT]) {
    _HASH_PREFIX(absorb)(c, key, HASH_LENGTH_TRIT);
    _HASH_PREFIX(squeeze)(c, key, HASH_LENGTH_TRIT);
    _HASH_PREFIX(reset)(c);
  }
  return 0;
}

int _ISS_PREFIX(digest)(trit_t *const chunk, int count, HASH_STATE *const c) {
  if (count >= 27 || count < 0) {
    return 1;
  }
  for (; count-- > 0;) {
    _HASH_PREFIX(absorb)(c, chunk, HASH_LENGTH_TRIT);
    _HASH_PREFIX(squeeze)(c, chunk, HASH_LENGTH_TRIT);
    _HASH_PREFIX(reset)(c);
  }
  return 0;
}

int _ISS_PREFIX(key_digest)(trit_t *key, trit_t *const digest,
                            size_t const key_length, HASH_STATE *const c) {
  if (key_length % HASH_LENGTH_TRIT) return -1;
  trit_t const *const k_start = key;

  for (trit_t *e = &key[key_length]; key < e; key = &key[HASH_LENGTH_TRIT]) {
    _ISS_PREFIX(digest)(key, 26, c);
  }
  _HASH_PREFIX(absorb)(c, k_start, key_length);
  _HASH_PREFIX(squeeze)(c, digest, HASH_LENGTH_TRIT);
  _HASH_PREFIX(reset)(c);
  return 0;
}

int _ISS_PREFIX(address)(trit_t *const address, trit_t const *const digest,
                         size_t const length, HASH_STATE *const c) {
  _HASH_PREFIX(absorb)(c, digest, length);
  _HASH_PREFIX(squeeze)(c, address, HASH_LENGTH_TRIT);
  _HASH_PREFIX(reset)(c);
  return 0;
}

int signed_window(trit_t const *hash) {
  ssize_t sum = 0, i;
  trit_t const *he;
  for (i = 1; i <= TRYTE_WIDTH; i++) {
    he = &hash[HASH_LENGTH_TRIT / TRYTE_WIDTH];
    do {
      sum += *hash;
    } while (++hash < he);
    if (sum == 0) {
      return i;
    }
  }
  return 0;
}

int _ISS_PREFIX(signature)(trit_t *sig, trit_t const *const hash,
                           size_t const start_offset, trit_t const *const key,
                           size_t const key_len, HASH_STATE *const c) {
  size_t window, i, j;
  int s;
  trit_t *se = &sig[key_len];
  if (sig != key) memcpy(sig, key, key_len * sizeof(trit_t));

  if (!(window = signed_window(hash))) {
    return 1;
  }

  for (i = start_offset % window; sig < se; i++) {
    for (j = i * ISS_CHUNK_LENGTH; j < (i + 1) * ISS_CHUNK_LENGTH && sig < se;
         j += TRYTE_WIDTH) {
      s = hash[j] + hash[j + 1] * 3 + hash[j + 2] * 9;
      _ISS_PREFIX(digest)(sig, TRYTE_VALUE_MAX - s, c);
      sig = &sig[HASH_LENGTH_TRIT];
    }
    i = (i + 1) % window;
  }
  return 0;
}

int _ISS_PREFIX(sig_digest)(trit_t *dig, trit_t const *const hash,
                            size_t const offset, trit_t *sig,
                            size_t const sig_len, HASH_STATE *const c) {
  size_t window, i, j;
  int s;
  trit_t *sig_start, *end;

  end = &sig[sig_len];
  if (!(window = signed_window(hash))) {
    return 1;
  }

  for (i = offset % window; sig < end;) {
    sig_start = sig;
    for (j = i * ISS_CHUNK_LENGTH; j < (i + 1) * ISS_CHUNK_LENGTH && sig < end;
         j += TRYTE_WIDTH) {
      s = hash[j] + hash[j + 1] * 3 + hash[j + 2] * 9;
      _ISS_PREFIX(digest)(sig, s - TRYTE_VALUE_MIN, c);
      sig = &sig[HASH_LENGTH_TRIT];
    }
    _HASH_PREFIX(absorb)(c, sig_start, ISS_KEY_LENGTH);
    _HASH_PREFIX(squeeze)(c, dig, HASH_LENGTH_TRIT);
    _HASH_PREFIX(reset)(c);
    dig = &dig[HASH_LENGTH_TRIT];
    i = (i + 1) % window;
  }
  return 0;
}

#undef _ISS_PREFIX
#undef _HASH_PREFIX

#undef CAT
#undef _CAT
