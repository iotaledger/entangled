#include <string.h>
#include "common/trinary/add.h"

#define MAX_TRYTE_VALUE 13
#define MIN_TRYTE_VALUE -13
#define TRYTE_WIDTH 3

#define CAT(A, ...) _CAT(A, __VA_ARGS__)
#define _CAT(A, ...) A ## __VA_ARGS__

#define _ISS_PREFIX(A) CAT(CAT(CAT(iss_,HASH_PREFIX),_),A)
#define _HASH_PREFIX(A) CAT(CAT(HASH_PREFIX,_),A)

int _ISS_PREFIX(subseed)(trit_t *const seed, trit_t *const out, int64_t index,
                HASH_STATE *const c) {
  memcpy(out, seed, HASH_LENGTH * sizeof(trit_t));
  add_assign(seed, HASH_LENGTH, index);
  _HASH_PREFIX(absorb)(c, seed, HASH_LENGTH);
  _HASH_PREFIX(squeeze)(c, out, HASH_LENGTH);
  _HASH_PREFIX(reset)(c);
  return 0;
}

int _ISS_PREFIX(key)(trit_t *const s, trit_t *key, const size_t key_length,
            HASH_STATE *const c) {
  if (key_length % HASH_LENGTH) return -1;
  trit_t *const e = &key[key_length];

  _HASH_PREFIX(absorb)(c, s, HASH_LENGTH);
  _HASH_PREFIX(squeeze)(c, key, key_length);
  _HASH_PREFIX(reset)(c);

  for (; key < e; key = &key[HASH_LENGTH]) {
    _HASH_PREFIX(absorb)(c, key, HASH_LENGTH);
    _HASH_PREFIX(squeeze)(c, key, HASH_LENGTH);
    _HASH_PREFIX(reset)(c);
  }
  return 0;
}

int _ISS_PREFIX(key_digest)(trit_t *key, trit_t *digest, const size_t key_length,
                   HASH_STATE *const c) {
  if (key_length % HASH_LENGTH) return -1;
  size_t i;
  trit_t *k_start = key;

  for (trit_t *e = &key[key_length]; key < e; key = &key[HASH_LENGTH]) {
    for (i = 0; i < 26; i++) {
      _HASH_PREFIX(absorb)(c, key, HASH_LENGTH);
      _HASH_PREFIX(squeeze)(c, key, HASH_LENGTH);
      _HASH_PREFIX(reset)(c);
    }
  }
  _HASH_PREFIX(absorb)(c, k_start, key_length);
  _HASH_PREFIX(squeeze)(c, digest, HASH_LENGTH);
  _HASH_PREFIX(reset)(c);
  return 0;
}

int _ISS_PREFIX(address)(trit_t *const d, trit_t *const digest, HASH_STATE *const c) {
  _HASH_PREFIX(absorb)(c, d, HASH_LENGTH);
  _HASH_PREFIX(squeeze)(c, digest, HASH_LENGTH);
  _HASH_PREFIX(reset)(c);
  return 0;
}

int hamming_window(trit_t *hash) {
  ssize_t sum = 0, i;
  trit_t *he;
  for (i = 1; i <= TRYTE_WIDTH; i++) {
    he = &hash[HASH_LENGTH / TRYTE_WIDTH];
    do {
      sum += *hash;
    } while (++hash < he);
    if (sum == 0) {
      return HASH_LENGTH * i / TRYTE_WIDTH;
    }
  }
  return 0;
}

int _ISS_PREFIX(signature)(trit_t *sig, trit_t *const hash, size_t start_offset, trit_t *key, size_t key_len,
                  HASH_STATE *const c) {
  size_t window, i, j;
  trit_t *se = &sig[key_len];
  if (sig != key) memcpy(sig, key, key_len * sizeof(trit_t));

  if(!(window = hamming_window(hash))) {
    return 1;
  }

  for (i = start_offset; sig < se; i++, sig = &sig[HASH_LENGTH]) {
    for (j = 0;
         j < MAX_TRYTE_VALUE - hash[i * TRYTE_WIDTH] +
                 hash[i * TRYTE_WIDTH + 1] * 3 + hash[i * TRYTE_WIDTH + 2] * 9;
         j++) {
      _HASH_PREFIX(absorb)(c, sig, HASH_LENGTH);
      memcpy(sig, c->state, HASH_LENGTH * sizeof(trit_t));
      _HASH_PREFIX(reset)(c);
    }
    if (i >= window) {
      i = 0;
    }
  }
  return 0;
}

int _ISS_PREFIX(sig_digest)(trit_t *const dig, trit_t *const hash, size_t offset, trit_t *sig,
                   size_t sig_len, HASH_STATE *const c) {
  size_t window, i, j;
  trit_t *sig_start = sig, *sig_end = &sig[sig_len];

  if(!(window = hamming_window(hash))) {
    return 1;
  }

  for (i = offset; sig < sig_end; i++, sig = &sig[HASH_LENGTH]) {
    for (j = 0; j < hash[i * TRYTE_WIDTH] + hash[i * TRYTE_WIDTH + 1] * 3 +
                        hash[i * TRYTE_WIDTH + 2] * 9 - MIN_TRYTE_VALUE;
         j++) {
      _HASH_PREFIX(absorb)(c, sig, HASH_LENGTH);
      memcpy(sig, c->state, HASH_LENGTH * sizeof(trit_t));
      _HASH_PREFIX(reset)(c);
    }
    if (i >= window) {
      i = 0;
    }
  }
  _HASH_PREFIX(absorb)(c, sig_start, sig_len);
  _HASH_PREFIX(squeeze)(c, dig, HASH_LENGTH);
  return 0;
}

#undef _ISS_PREFIX
#undef _HASH_PREFIX

#undef CAT
#undef _CAT
