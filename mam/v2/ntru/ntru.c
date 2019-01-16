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
\file ntru.c
\brief MAM2 NTRU layer.
*/

#include <memory.h>

#include "mam/v2/alloc.h"
#include "mam/v2/ntru/ntru.h"
#include "mam/v2/ntru/poly.h"

trits_t ntru_id_trits(ntru_t *n) {
  return trits_from_rep(MAM2_NTRU_ID_SIZE, n->id);
}

trits_t ntru_sk_trits(ntru_t *n) {
  return trits_from_rep(MAM2_NTRU_SK_SIZE, n->sk);
}

void ntru_load_sk(ntru_t *n) {
  poly_coeff_t *f;
  f = (poly_coeff_t *)n->f;

  poly_small_from_trits(f, ntru_sk_trits(n));
  /* f := NTT(1+3f) */
  poly_small_mul3(f, f);
  poly_small3_add1(f);
  poly_ntt(f, f);
}

void ntru_gen(ntru_t *n, prng_t *p, trits_t N, trits_t pk) {
  MAM2_TRITS_DEF0(i, 81);
  MAM2_TRITS_DEF0(r, 2 * MAM2_NTRU_SK_SIZE);
  poly_coeff_t *f;
  MAM2_POLY_DEF(g);
  MAM2_POLY_DEF(h);
  i = MAM2_TRITS_INIT(i, 81);
  r = MAM2_TRITS_INIT(r, 2 * MAM2_NTRU_SK_SIZE);
  f = (poly_coeff_t *)n->f;

  trits_set_zero(i);
  do {
    prng_gen2(p, MAM2_PRNG_DST_NTRUKEY, N, i, r);
    poly_small_from_trits(f, trits_take(r, MAM2_NTRU_SK_SIZE));
    poly_small_from_trits(g, trits_drop(r, MAM2_NTRU_SK_SIZE));

    /* f := NTT(1+3f) */
    poly_small_mul3(f, f);
    poly_small3_add1(f);
    poly_ntt(f, f);

    /* g := NTT(3g) */
    poly_small_mul3(g, g);
    poly_ntt(g, g);
  } while (!(poly_has_inv(f) && poly_has_inv(g)) && trits_inc(i));

  /* h := 3g/(1+3f) */
  poly_inv(f, h);
  poly_conv(g, h, h);
  poly_intt(h, h);

  poly_to_trits(h, pk);
  trits_copy(trits_take(r, MAM2_NTRU_SK_SIZE), ntru_sk_trits(n));

  trits_copy(trits_take(pk, MAM2_NTRU_ID_SIZE), ntru_id_trits(n));

  trits_set_zero(i);
  trits_set_zero(r);
}

void ntru_encr_r(trits_t pk, spongos_t *s, trits_t r, trits_t K, trits_t Y) {
  bool_t ok = 0;
  MAM2_POLY_DEF(h);
  MAM2_POLY_DEF(t);

  MAM2_ASSERT(trits_size(r) == MAM2_NTRU_SK_SIZE);
  MAM2_ASSERT(trits_size(K) == MAM2_SPONGE_KEY_SIZE);
  MAM2_ASSERT(trits_size(Y) == MAM2_NTRU_EKEY_SIZE);

  ok = poly_from_trits(h, pk);
  MAM2_ASSERT(ok);
  poly_ntt(h, h);

  /* t(x) := r(x)*h(x) */
  poly_small_from_trits(t, r);
  poly_ntt(t, t);
  poly_conv(t, h, t);
  poly_intt(t, t);

  /* h(x) = AE(r*h;K) */
  poly_to_trits(t, Y);
  spongos_init(s);
  spongos_absorb(s, Y);
  spongos_commit(s);
  spongos_encr(s, K, trits_take(r, MAM2_SPONGE_KEY_SIZE));
  spongos_squeeze(s, trits_drop(r, MAM2_SPONGE_KEY_SIZE));
  poly_small_from_trits(h, r);

  /* Y = r*h + AE(r*h;K) */
  poly_add(t, h, t);
  poly_to_trits(t, Y);

  memset(h, 0, sizeof(h));
}

void ntru_encr(trits_t pk, prng_t *p, spongos_t *s, trits_t K, trits_t N,
               trits_t Y) {
  trits_t r;

  MAM2_ASSERT(trits_size(K) == MAM2_SPONGE_KEY_SIZE);
  MAM2_ASSERT(trits_size(Y) == MAM2_NTRU_EKEY_SIZE);
  MAM2_ASSERT(!trits_is_same(K, Y));

  r = trits_take(Y, MAM2_NTRU_SK_SIZE);
  prng_gen2(p, MAM2_PRNG_DST_NTRUKEY, K, N, r);
  ntru_encr_r(pk, s, r, K, Y);
}

bool_t ntru_decr(ntru_t *n, spongos_t *s, trits_t Y, trits_t K) {
  bool_t b;
  poly_coeff_t *f;
  MAM2_POLY_DEF(t);
  MAM2_POLY_DEF(r);
  MAM2_TRITS_DEF0(kt, MAM2_NTRU_SK_SIZE);
  MAM2_TRITS_DEF0(rh, MAM2_NTRU_EKEY_SIZE);
  MAM2_TRITS_DEF0(m, MAM2_NTRU_SK_SIZE - MAM2_NTRU_KEY_SIZE);
  kt = MAM2_TRITS_INIT(kt, MAM2_NTRU_SK_SIZE);
  rh = MAM2_TRITS_INIT(rh, MAM2_NTRU_EKEY_SIZE);
  m = MAM2_TRITS_INIT(m, MAM2_NTRU_SK_SIZE - MAM2_NTRU_KEY_SIZE);

  MAM2_ASSERT(trits_size(K) == MAM2_SPONGE_KEY_SIZE);
  MAM2_ASSERT(trits_size(Y) == MAM2_NTRU_EKEY_SIZE);

  /* f is NTT form */
  f = (poly_coeff_t *)n->f;

  /* t(x) := Y */
  if (!poly_from_trits(t, Y)) return 0;

  /* r(x) := t(x)*(1+3f(x)) (mods 3) */
  poly_ntt(t, r);
  poly_conv(r, f, r);
  poly_intt(r, r);
  poly_round_to_trits(r, kt);
  poly_small_from_trits(r, kt);

  /* t(x) := Y - r(x) */
  poly_sub(t, r, t);
  poly_to_trits(t, rh);

  /* K = AD(rh;kt) */
  spongos_init(s);
  spongos_absorb(s, rh);
  spongos_commit(s);
  spongos_decr(s, trits_take(kt, MAM2_NTRU_KEY_SIZE), K);
  spongos_squeeze(s, m);
  b = trits_cmp_eq(m, trits_drop(kt, MAM2_NTRU_KEY_SIZE));

  trits_set_zero(kt);
  trits_set_zero(rh);
  trits_set_zero(m);
  memset(t, 0, sizeof(t));
  memset(r, 0, sizeof(r));
  return b;
}

retcode_t ntru_create(ialloc *a, ntru_t *n) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  word_t *id = 0, *sk = 0;
  void *f = 0;

  MAM2_ASSERT(n);

  do {
    id = mam_words_alloc(a, MAM2_WORDS(MAM2_NTRU_ID_SIZE));
    err_guard(id, RC_OOM);
    sk = mam_words_alloc(a, MAM2_WORDS(MAM2_NTRU_SK_SIZE));
    err_guard(sk, RC_OOM);
    f = mam_alloc(a, sizeof(poly_t));
    err_guard(f, RC_OOM);

    n->id = id;
    n->sk = sk;
    n->f = f;
    id = 0;
    sk = 0;
    f = 0;
    e = RC_OK;
  } while (0);

  mam_words_free(a, id);
  mam_words_free(a, sk);
  mam_free(a, f);
  return e;
}

void ntru_destroy(ialloc *a, ntru_t *n) {
  MAM2_ASSERT(n);
  mam_words_free(a, n->id);
  n->id = 0;
  mam_words_free(a, n->sk);
  n->sk = 0;
  mam_free(a, n->f);
  n->f = 0;
}
