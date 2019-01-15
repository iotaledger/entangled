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

#include "mam/v2/ntru/ntru.h"
#include "mam/v2/ntru/poly.h"

trits_t ntru_id_trits(intru *n) {
  return trits_from_rep(MAM2_NTRU_ID_SIZE, n->id);
}

trits_t ntru_sk_trits(intru *n) {
  return trits_from_rep(MAM2_NTRU_SK_SIZE, n->sk);
}

void ntru_load_sk(intru *n) {
  poly_coeff_t *f;
  f = (poly_coeff_t *)n->f;

  poly_small_from_trits(f, ntru_sk_trits(n));
  /* f := NTT(1+3f) */
  poly_small_mul3(f, f);
  poly_small3_add1(f);
  poly_ntt(f, f);
}

void ntru_gen(intru *n, iprng *p, trits_t N, trits_t pk) {
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

static void ntru_encr_r(trits_t pk, ispongos *s, trits_t r, trits_t K,
                        trits_t Y) {
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

void ntru_encr(trits_t pk, iprng *p, ispongos *s, trits_t K, trits_t N,
               trits_t Y) {
  trits_t r;

  MAM2_ASSERT(trits_size(K) == MAM2_SPONGE_KEY_SIZE);
  MAM2_ASSERT(trits_size(Y) == MAM2_NTRU_EKEY_SIZE);
  MAM2_ASSERT(!trits_is_same(K, Y));

  r = trits_take(Y, MAM2_NTRU_SK_SIZE);
  prng_gen2(p, MAM2_PRNG_DST_NTRUKEY, K, N, r);
  ntru_encr_r(pk, s, r, K, Y);
}

bool_t ntru_decr(intru *n, ispongos *s, trits_t Y, trits_t K) {
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

err_t ntru_create(ialloc *a, intru *n) {
  err_t e = err_internal_error;
  word_t *id = 0, *sk = 0;
  void *f = 0;

  MAM2_ASSERT(n);

  do {
    id = mam_words_alloc(a, MAM2_WORDS(MAM2_NTRU_ID_SIZE));
    err_guard(id, err_bad_alloc);
    sk = mam_words_alloc(a, MAM2_WORDS(MAM2_NTRU_SK_SIZE));
    err_guard(sk, err_bad_alloc);
    f = mam_alloc(a, sizeof(poly_t));
    err_guard(f, err_bad_alloc);

    n->id = id;
    n->sk = sk;
    n->f = f;
    id = 0;
    sk = 0;
    f = 0;
    e = err_ok;
  } while (0);

  mam_words_free(a, id);
  mam_words_free(a, sk);
  mam_free(a, f);
  return e;
}

void ntru_destroy(ialloc *a, intru *n) {
  MAM2_ASSERT(n);
  mam_words_free(a, n->id);
  n->id = 0;
  mam_words_free(a, n->sk);
  n->sk = 0;
  mam_free(a, n->f);
  n->f = 0;
}

/* \brief This special trivial spongos transform is used
in order to get `AE(s;k)=1^n`. */
static void ntru_test_f(void *buf, word_t *s) {
  trits_t x = trits_from_rep(MAM2_SPONGE_RATE, s);
  trits_set1(x, 1);
}

static void ntru_test_gen(intru *n, trits_t rf, trits_t rg, trits_t pk) {
  poly_coeff_t *f;
  MAM2_POLY_DEF(g);
  MAM2_POLY_DEF(h);

  f = (poly_coeff_t *)n->f;
  poly_small_from_trits(f, rf);
  poly_small_from_trits(g, rg);

  /* f := NTT(1+3f) */
  poly_small_mul3(f, f);
  poly_small3_add1(f);
  poly_ntt(f, f);

  /* g := NTT(3g) */
  poly_small_mul3(g, g);
  poly_ntt(g, g);

  MAM2_ASSERT(poly_has_inv(f) && poly_has_inv(g));

  /* h := NTT(3g/(1+3f)) */
  poly_inv(f, h);
  poly_conv(g, h, h);

  poly_to_trits(h, pk);
}

bool_t ntru_test(intru *n, ispongos *s, iprng *p) {
  bool_t r = 1;
  size_t i;
  size_t test_count = 30;
  poly_coeff_t *f0;
  MAM2_POLY_DEF(f);
  MAM2_TRITS_DEF0(nonce, 3 * 10);
  MAM2_TRITS_DEF0(pk, MAM2_NTRU_PK_SIZE);
  MAM2_TRITS_DEF0(key, MAM2_NTRU_KEY_SIZE);
  MAM2_TRITS_DEF0(ekey, MAM2_NTRU_EKEY_SIZE);
  MAM2_TRITS_DEF0(dekey, MAM2_NTRU_KEY_SIZE);
  nonce = MAM2_TRITS_INIT(nonce, 3 * 10);
  pk = MAM2_TRITS_INIT(pk, MAM2_NTRU_PK_SIZE);
  key = MAM2_TRITS_INIT(key, MAM2_NTRU_KEY_SIZE);
  ekey = MAM2_TRITS_INIT(ekey, MAM2_NTRU_EKEY_SIZE);
  dekey = MAM2_TRITS_INIT(dekey, MAM2_NTRU_KEY_SIZE);
  f0 = (poly_coeff_t *)n->f;

  trits_from_str(key,
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC");
  /* it's safe to reuse sponge from spongos for prng */
  /* as spongos is exclusively used in ntru_encr/ntru_decr. */
  prng_init(p, s->s, key);

  /* decryption failure special test case */
  {
    void (*sf)(void *, word_t *);
    MAM2_TRITS_DEF0(u, MAM2_NTRU_SK_SIZE);
    MAM2_TRITS_DEF0(v, MAM2_NTRU_SK_SIZE);
    u = MAM2_TRITS_INIT(u, MAM2_NTRU_SK_SIZE);
    v = MAM2_TRITS_INIT(v, MAM2_NTRU_SK_SIZE);

    /* save current spongos transform */
    sf = s->s->f;
    /* and use a special one for this case */
    s->s->f = ntru_test_f;

    trits_set1(v, 1);
    trits_set1(trits_drop(v, 512), -1);
    ntru_test_gen(n, v, v, pk);

    trits_set1(u, 1);
    trits_set1(key, 0);
    /* encrypt with (1+x+x^2+..) as noise */
    /* key=0^n so that encr(s;key)=1^n */
    ntru_encr_r(pk, s, u, key, ekey);

    /* decryption should fail! as there will be overflow in 511-th coeff */
    r = r && !ntru_decr(n, s, ekey, dekey);
    /* failure is detected by ntru_decr by checking tag */

    /* but once any of keys has at least one 0 decryption should succeed */
    trits_put1(u, 0);
    ntru_encr_r(pk, s, u, key, ekey);
    r = r && ntru_decr(n, s, ekey, dekey);
    trits_put1(u, 1);

    trits_put1(key, 1);
    ntru_encr_r(pk, s, u, key, ekey);
    r = r && ntru_decr(n, s, ekey, dekey);
    trits_put1(key, 0);

    trits_copy(v, u);
    trits_put1(trits_drop(v, 511), 0);
    ntru_test_gen(n, u, v, pk);
    trits_set1(u, 1);
    trits_put1(u, 0);
    ntru_encr_r(pk, s, u, key, ekey);
    r = r && ntru_decr(n, s, ekey, dekey);
    trits_put1(u, 1);

    /* restore the original spongos transform */
    s->s->f = sf;
  }

  i = 0;
  trits_set_zero(key);
  do {
    r = trits_from_str(nonce, "NONCE9PK99");
    MAM2_ASSERT(r);
    trits_set_zero(pk);
    trits_put1(pk, 1);
    poly_small_from_trits(f, trits_take(pk, MAM2_NTRU_SK_SIZE));
    ntru_gen(n, p, nonce, pk);
    poly_add(f, f0, f);

    do {
      trits_from_str(nonce, "NONCE9ENC9");
      ntru_encr(pk, p, s, key, nonce, ekey);

      r = r && ntru_decr(n, s, ekey, dekey);
      r = r && trits_cmp_eq(key, dekey);

      trits_put1(ekey, trit_add(trits_get1(ekey), 1));
      r = r && !ntru_decr(n, s, ekey, dekey);
      r = r && !trits_cmp_eq(key, dekey);
      trits_put1(ekey, trit_sub(trits_get1(ekey), 1));

      /*trits_put1(ntru_sk_trits(n), trit_add(trits_get1(ntru_sk_trits(n)),
       * 1));*/
      n->f = f;
      r = r && !ntru_decr(n, s, ekey, dekey);
      r = r && !trits_cmp_eq(key, dekey);
      n->f = f0;
      /*trits_put1(ntru_sk_trits(n), trit_sub(trits_get1(ntru_sk_trits(n)),
       * 1));*/

      trits_from_str(nonce, "NONCE9KEY9");
      prng_gen(p, MAM2_PRNG_DST_SECKEY, nonce, key);
    } while (r && (0 != (++i % (test_count / 10))));
  } while (r && (++i < test_count));

  return r;
}
