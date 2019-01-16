/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "mam/v2/ntru/ntru.h"
#include "mam/v2/ntru/poly.h"

/* \brief This special trivial spongos transform is used
in order to get `AE(spongos;key)=1^n`. */
static void ntru_test_f(void *buf, word_t *s) {
  trits_t x = trits_from_rep(MAM2_SPONGE_RATE, s);
  trits_set1(x, 1);
}

static void ntru_test_gen(ntru_t *n, trits_t rf, trits_t rg, trits_t pk) {
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

bool_t ntru_test(ntru_t *n, spongos_t *s, prng_t *p) {
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
  /* it'spongos safe to reuse sponge from spongos for prng */
  /* as spongos is exclusively used in ntru_encr/ntru_decr. */
  prng_init(p, s->sponge, key);

  /* decryption failure special test case */
  {
    void (*sf)(void *, word_t *);
    MAM2_TRITS_DEF0(u, MAM2_NTRU_SK_SIZE);
    MAM2_TRITS_DEF0(v, MAM2_NTRU_SK_SIZE);
    u = MAM2_TRITS_INIT(u, MAM2_NTRU_SK_SIZE);
    v = MAM2_TRITS_INIT(v, MAM2_NTRU_SK_SIZE);

    /* save current spongos transform */
    sf = s->sponge->f;
    /* and use a special one for this case */
    s->sponge->f = ntru_test_f;

    trits_set1(v, 1);
    trits_set1(trits_drop(v, 512), -1);
    ntru_test_gen(n, v, v, pk);

    trits_set1(u, 1);
    trits_set1(key, 0);
    /* encrypt with (1+x+x^2+..) as noise */
    /* key=0^n so that encr(spongos;key)=1^n */
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
    s->sponge->f = sf;
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

int main(void) {
  UNITY_BEGIN();

  return UNITY_END();
}
