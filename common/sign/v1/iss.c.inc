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
                HASH_STATE *const state) {
  memcpy(out, seed, HASH_LENGTH * sizeof(trit_t));
  add_assign(seed, HASH_LENGTH, index);
  _HASH_PREFIX(absorb)(state, seed, HASH_LENGTH);
  _HASH_PREFIX(squeeze)(state, out, HASH_LENGTH);
  _HASH_PREFIX(reset)(state);
  return 0;
}

int _ISS_PREFIX(key)(trit_t *const subseed, trit_t *key, const size_t key_length,
            HASH_STATE *const state) {
  if (key_length % HASH_LENGTH) return -1;
  trit_t *const end = &key[key_length];

  _HASH_PREFIX(absorb)(state, subseed, HASH_LENGTH);

  for (; key < end; key = &key[HASH_LENGTH]) {
    _HASH_PREFIX(squeeze)(state, key, HASH_LENGTH);
  }

  _HASH_PREFIX(reset)(state);

  return 0;
}

int _ISS_PREFIX(key_digest)(trit_t *key, trit_t *digest, const size_t key_length,
                   HASH_STATE *const state) {
  if (key_length % HASH_LENGTH) return -1;

  size_t i;
  trit_t *const k_start = key;
  trit_t *const end = &key[key_length];

  for (; key < end; key = &key[HASH_LENGTH]) {
    for (i = 0; i < 26; i++) {
      _HASH_PREFIX(absorb)(state, key, HASH_LENGTH);
      _HASH_PREFIX(squeeze)(state, key, HASH_LENGTH);
      _HASH_PREFIX(reset)(state);
    }
  }
  _HASH_PREFIX(absorb)(state, k_start, key_length);
  _HASH_PREFIX(squeeze)(state, digest, HASH_LENGTH);
  _HASH_PREFIX(reset)(state);
  return 0;
}

int _ISS_PREFIX(address)(trit_t *const digest, trit_t *const address, HASH_STATE *const state) {
  _HASH_PREFIX(absorb)(state, digest, HASH_LENGTH);
  _HASH_PREFIX(squeeze)(state, address, HASH_LENGTH);
  _HASH_PREFIX(reset)(state);
  return 0;
}

int _ISS_PREFIX(signature)(trit_t *sig, trit_t *const hash, trit_t *key, size_t key_len,
                  HASH_STATE *const state) {
  size_t i, j;
  trit_t *se = &sig[key_len];
  if (sig != key) memcpy(sig, key, key_len * sizeof(trit_t));
  for (i = 0; sig < se; i++, sig = &sig[HASH_LENGTH]) {
    for (j = 0;
         j < MAX_TRYTE_VALUE - hash[i * TRYTE_WIDTH] +
                 hash[i * TRYTE_WIDTH + 1] * 3 + hash[i * TRYTE_WIDTH + 2] * 9;
         j++) {
      _HASH_PREFIX(absorb)(state, sig, HASH_LENGTH);
      memcpy(sig, state->state, HASH_LENGTH * sizeof(trit_t));
      _HASH_PREFIX(reset)(state);
    }
  }
  return 0;
}

int _ISS_PREFIX(sig_digest)(trit_t *const dig, trit_t *const hash, trit_t *sig,
                   size_t sig_len, HASH_STATE *const state) {
  size_t i, j;
  trit_t *sig_start = sig, *sig_end = &sig[sig_len];
  for (i = 0; sig < sig_end; i++, sig = &sig[HASH_LENGTH]) {
    for (j = 0; j < hash[i * TRYTE_WIDTH] + hash[i * TRYTE_WIDTH + 1] * 3 +
                        hash[i * TRYTE_WIDTH + 2] * 9 - MIN_TRYTE_VALUE;
         j++) {
      _HASH_PREFIX(absorb)(state, sig, HASH_LENGTH);
      memcpy(sig, state->state, HASH_LENGTH * sizeof(trit_t));
      _HASH_PREFIX(reset)(state);
    }
  }
  _HASH_PREFIX(absorb)(state, sig_start, sig_len);
  _HASH_PREFIX(squeeze)(state, dig, HASH_LENGTH);
  return 0;
}

#undef _ISS_PREFIX
#undef _HASH_PREFIX

#undef CAT
#undef _CAT