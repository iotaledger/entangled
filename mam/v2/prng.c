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
#include "prng.h"

#include <memory.h>
#include <string.h>

static void prng_absorbn(isponge *s, size_t n, trits_t *KdN) {
  sponge_init(s);
  sponge_absorbn(s, MAM2_SPONGE_CTL_KEY, n, KdN);
}

static void prng_squeeze(isponge *s, trits_t Y) {
  sponge_squeeze(s, MAM2_SPONGE_CTL_PRN, Y);
}

static trits_t prng_key_trits(iprng *p) {
  return trits_from_rep(MAM2_PRNG_KEY_SIZE, p->k);
}

void prng_init(iprng *p, isponge *s, trits_t K) {
  MAM2_ASSERT(trits_size(K) == MAM2_PRNG_KEY_SIZE);

  p->s = s;
  trits_copy(K, prng_key_trits(p));
}

void prng_gen(iprng *p, trint3_t d, trits_t N, trits_t Y) {
  MAM2_TRITS_DEF0(dt, 3);
  trits_t KdN[3];
  dt = MAM2_TRITS_INIT(dt, 3);

  KdN[0] = prng_key_trits(p);
  KdN[1] = dt;
  KdN[2] = N;
  trits_put3(dt, d);
  prng_absorbn(p->s, 3, KdN);
  prng_squeeze(p->s, Y);
}

void prng_gen2(iprng *p, trint3_t d, trits_t N1, trits_t N2, trits_t Y) {
  MAM2_TRITS_DEF0(dt, 3);
  trits_t KdN[4];
  dt = MAM2_TRITS_INIT(dt, 3);

  KdN[0] = prng_key_trits(p);
  KdN[1] = dt;
  KdN[2] = N1;
  KdN[3] = N2;
  trits_put3(dt, d);
  prng_absorbn(p->s, 4, KdN);
  prng_squeeze(p->s, Y);
}

void prng_gen3(iprng *p, trint3_t d, trits_t N1, trits_t N2, trits_t N3,
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
  prng_absorbn(p->s, 5, KdN);
  prng_squeeze(p->s, Y);
}

void prng_gen_str(iprng *p, trint3_t d, char const *nonce, trits_t Y) {
  size_t n;
  MAM2_TRITS_DEF0(N, MAM2_SPONGE_RATE);
  N = MAM2_TRITS_INIT(N, MAM2_SPONGE_RATE);

  n = strlen(nonce) * 3;
  N = trits_take_min(N, n);
  trits_from_str(N, nonce);

  prng_gen(p, d, N, Y);
}

err_t prng_create(ialloc *a, iprng *p) {
  err_t e = err_internal_error;
  MAM2_ASSERT(p);
  do {
    memset(p, 0, sizeof(iprng));
    p->k = mam_words_alloc(a, MAM2_WORDS(MAM2_PRNG_KEY_SIZE));
    err_guard(p->k, err_bad_alloc);
    e = err_ok;
  } while (0);
  return e;
}

void prng_destroy(ialloc *a, iprng *p) {
  MAM2_ASSERT(p);
  mam_words_free(a, p->k);
  p->k = 0;
}

bool_t prng_test(iprng *p) {
  bool_t r = 1;
  MAM2_TRITS_DEF0(K, MAM2_PRNG_KEY_SIZE);
  MAM2_TRITS_DEF0(N, 18);
  MAM2_TRITS_DEF0(Y1, 243 * 2 + 18);
  MAM2_TRITS_DEF0(Y2, 243 * 2 + 18);
  K = MAM2_TRITS_INIT(K, MAM2_PRNG_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, 18);
  Y1 = MAM2_TRITS_INIT(Y1, 243 * 2 + 18);
  Y2 = MAM2_TRITS_INIT(Y2, 243 * 2 + 18);

  /* init K */
  trits_set_zero(K);
  trits_from_str(K,
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM");
  sponge_init(p->s);
  sponge_absorb(p->s, MAM2_SPONGE_CTL_KEY, K);
  sponge_squeeze(p->s, MAM2_SPONGE_CTL_KEY, K);
  /* init N */
  trits_set_zero(N);

  prng_init(p, p->s, K);
  prng_gen(p, 0, N, Y1);
  prng_gen(p, 1, N, Y2);
  r = r && !trits_cmp_eq(Y1, Y2);

  return r;
}
