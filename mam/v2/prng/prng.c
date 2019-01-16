/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file prng.c
\brief MAM2 PRNG layer.
*/

#include <memory.h>
#include <string.h>

#include "mam/v2/prng/prng.h"

static void prng_absorbn(sponge_t *s, size_t n, trits_t *KdN) {
  sponge_init(s);
  sponge_absorbn(s, MAM2_SPONGE_CTL_KEY, n, KdN);
}

static void prng_squeeze(sponge_t *s, trits_t Y) {
  sponge_squeeze(s, MAM2_SPONGE_CTL_PRN, Y);
}

static trits_t prng_key_trits(prng_t *p) {
  return trits_from_rep(MAM2_PRNG_KEY_SIZE, p->key);
}

void prng_init(prng_t *p, sponge_t *s, trits_t K) {
  MAM2_ASSERT(trits_size(K) == MAM2_PRNG_KEY_SIZE);

  p->sponge = s;
  trits_copy(K, prng_key_trits(p));
}

void prng_gen(prng_t *p, trint3_t d, trits_t N, trits_t Y) {
  MAM2_TRITS_DEF0(dt, 3);
  trits_t KdN[3];
  dt = MAM2_TRITS_INIT(dt, 3);

  KdN[0] = prng_key_trits(p);
  KdN[1] = dt;
  KdN[2] = N;
  trits_put3(dt, d);
  prng_absorbn(p->sponge, 3, KdN);
  prng_squeeze(p->sponge, Y);
}

void prng_gen2(prng_t *p, trint3_t d, trits_t N1, trits_t N2, trits_t Y) {
  MAM2_TRITS_DEF0(dt, 3);
  trits_t KdN[4];
  dt = MAM2_TRITS_INIT(dt, 3);

  KdN[0] = prng_key_trits(p);
  KdN[1] = dt;
  KdN[2] = N1;
  KdN[3] = N2;
  trits_put3(dt, d);
  prng_absorbn(p->sponge, 4, KdN);
  prng_squeeze(p->sponge, Y);
}

void prng_gen3(prng_t *p, trint3_t d, trits_t N1, trits_t N2, trits_t N3,
               trits_t Y) {
  MAM2_TRITS_DEF0(dt, 3);
  trits_t KdN[5];
  dt = MAM2_TRITS_INIT(dt, 3);

  KdN[0] = prng_key_trits(p);
  KdN[1] = dt;
  KdN[2] = N1;
  KdN[3] = N2;
  KdN[4] = N3;
  trits_put3(dt, d);
  prng_absorbn(p->sponge, 5, KdN);
  prng_squeeze(p->sponge, Y);
}

void prng_gen_str(prng_t *p, trint3_t d, char const *nonce, trits_t Y) {
  size_t n;
  MAM2_TRITS_DEF0(N, MAM2_SPONGE_RATE);
  N = MAM2_TRITS_INIT(N, MAM2_SPONGE_RATE);

  n = strlen(nonce) * 3;
  N = trits_take_min(N, n);
  trits_from_str(N, nonce);

  prng_gen(p, d, N, Y);
}

err_t prng_create(ialloc *a, prng_t *p) {
  err_t e = err_internal_error;
  MAM2_ASSERT(p);
  do {
    memset(p, 0, sizeof(prng_t));
    p->key = mam_words_alloc(a, MAM2_WORDS(MAM2_PRNG_KEY_SIZE));
    err_guard(p->key, err_bad_alloc);
    e = err_ok;
  } while (0);
  return e;
}

void prng_destroy(ialloc *a, prng_t *p) {
  MAM2_ASSERT(p);
  mam_words_free(a, p->key);
  p->key = 0;
}
