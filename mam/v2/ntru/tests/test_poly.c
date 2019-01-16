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

#include "mam/v2/ntru/poly.h"
#include "mam/v2/ntru/poly_param.h"

bool_t poly_test_ntt() {
  bool_t r = 1;
  size_t i, k, ki;
  poly_t f, g, fg, tf, tg, tfg, itf, itg, itfg, tif, itif, fitif, t2, it2;
  poly_coeff_t c = 7, a = 13, n = MAM2_POLY_N, two = 1;

  for (i = 0; i < MAM2_POLY_N; ++i, two = poly_coeff_mul(two, 2))
    f[i] = 0,     /*two,*/
        g[i] = 0; /*poly_coeff_add(two, 1233);*/

  f[0] = MAM2_POLY_COEFF_ONE; /* f(x) = 1 */
  g[1] = MAM2_POLY_COEFF_ONE; /* g(x) = x */

  for (k = ki = 0; r && k < 100; ++k) {
    poly_ntt2(f, t2);
    poly_intt2(t2, it2);
    poly_ntt(f, tf);
    poly_intt(tf, itf);
    MAM2_ASSERT(poly_is_eq(f, itf));

    poly_ntt2(g, t2);
    poly_intt2(t2, it2);
    poly_ntt(g, tg);
    poly_intt(tg, itg);
    MAM2_ASSERT(poly_is_eq(g, itg));

    poly_mul(f, g, fg);
    poly_ntt(g, tg);
    poly_conv(tf, tg, tfg);
    poly_intt(tfg, itfg);
    MAM2_ASSERT(poly_is_eq(fg, itfg));

    if (poly_has_inv(tf)) {
      ++ki;
      poly_inv(tf, tif);
      poly_intt(tif, itif);
      poly_mul(f, itif, fitif);
      MAM2_ASSERT(poly_is_one(fitif));
    }

    for (i = 0; i < MAM2_POLY_N; ++i)
      f[i] = poly_coeff_add(a, poly_coeff_mul(c, f[i]));
  }

  dbg_printf("key=%d\tki=%d\n", k, ki);
  return r;
}

bool_t poly_test_coeff_mul() {
  bool_t r = 1;
  size_t i, j;
  poly_coeff_t a, b, m;
  trint9_t t, n;

  for (i = 0; i <= MAM2_POLY_Q / 2; ++i)
    for (j = 0; j <= MAM2_POLY_Q / 2; ++j) {
      a = poly_coeff_from_trint9((trint9_t)i);
      b = poly_coeff_from_trint9((trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM2_POLY_Q;
      t = t <= MAM2_POLY_Q / 2 ? t : t - MAM2_POLY_Q;
      MAM2_ASSERT(n == t);

      a = poly_coeff_from_trint9(-(trint9_t)i);
      b = poly_coeff_from_trint9((trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM2_POLY_Q;
      t = t <= MAM2_POLY_Q / 2 ? -t : MAM2_POLY_Q - t;
      MAM2_ASSERT(n == t);

      a = poly_coeff_from_trint9((trint9_t)i);
      b = poly_coeff_from_trint9(-(trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM2_POLY_Q;
      t = t <= MAM2_POLY_Q / 2 ? -t : MAM2_POLY_Q - t;
      MAM2_ASSERT(n == t);

      a = poly_coeff_from_trint9(-(trint9_t)i);
      b = poly_coeff_from_trint9(-(trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM2_POLY_Q;
      t = t <= MAM2_POLY_Q / 2 ? t : t - MAM2_POLY_Q;
      MAM2_ASSERT(n == t);
    }
  return r;
}

bool_t poly_test_coeff() {
  bool_t r = 1;
  size_t i;
#define MAM2_POLY_TEST_N 201
  trint9_t tt[MAM2_POLY_TEST_N];

  size_t ia, ib, ic;
  poly_coeff_t a, b, c, m;
  trint9_t t;
  trint18_t s;

  for (i = 0; i < MAM2_POLY_TEST_N / 4; ++i)
    tt[i] = (trint9_t)(-(MAM2_POLY_Q - 1) / 2 + i);
  for (; i < MAM2_POLY_TEST_N / 2; ++i)
    tt[i] = (trint9_t)(MAM2_POLY_TEST_N / 2 - i);
  tt[i++] = 0;
  for (; i < MAM2_POLY_TEST_N; ++i) tt[i] = -tt[MAM2_POLY_TEST_N - 1 - i];

  for (ia = 0; ia < MAM2_POLY_TEST_N; ++ia) {
    a = poly_coeff_from_trint9(tt[ia]);
    t = poly_coeff_to_trint9(a);
    r = r && (tt[ia] == t);
    MAM2_ASSERT(r = r);

    for (ib = 0; ib < MAM2_POLY_TEST_N; ++ib) {
      b = poly_coeff_from_trint9(tt[ib]);

      m = poly_coeff_add(a, b);
      t = poly_coeff_to_trint9(m);
      s = (trint18_t)tt[ia] + tt[ib];
      s = MAM2_MODS(s, (MAM2_POLY_Q * MAM2_POLY_Q), MAM2_POLY_Q);
      r = r && t == s;
      MAM2_ASSERT(r = r);

      m = poly_coeff_sub(a, b);
      t = poly_coeff_to_trint9(m);
      s = (trint18_t)tt[ia] - tt[ib];
      s = MAM2_MODS(s, (MAM2_POLY_Q * MAM2_POLY_Q), MAM2_POLY_Q);
      r = r && t == s;
      MAM2_ASSERT(r = r);

      m = poly_coeff_mul(a, b);
      t = poly_coeff_to_trint9(m);
      s = (trint18_t)tt[ia] * tt[ib];
      s = MAM2_MODS(s, (MAM2_POLY_Q * MAM2_POLY_Q), MAM2_POLY_Q);
      r = r && t == s;
      MAM2_ASSERT(r = r);

      for (ic = 0; ic < MAM2_POLY_TEST_N; ++ic) {
        c = poly_coeff_from_trint9(tt[ic]);
        m = poly_coeff_mul_add(a, b, c);
        t = poly_coeff_to_trint9(m);
        s = (trint18_t)tt[ia] * tt[ib] + tt[ic];
        s = MAM2_MODS(s, (MAM2_POLY_Q * MAM2_POLY_Q), MAM2_POLY_Q);
        r = r && t == s;
        MAM2_ASSERT(r = r);
      }
    }
  }
#undef MAM2_POLY_TEST_N
  return r;
}

bool_t poly_test_ntrufail() {
  MAM2_POLY_DEF(f);
  MAM2_POLY_DEF(g);
  MAM2_POLY_DEF(h);
  MAM2_POLY_DEF(t);
  MAM2_POLY_DEF(s);
  MAM2_TRITS_DEF0(r, 1024);
  r = MAM2_TRITS_INIT(r, 1024);

  trits_set1(r, 1);
  trits_set1(trits_drop(r, 512), -1);
  poly_small_from_trits(f, r);
  poly_small_from_trits(g, r);

  /* NTT(1+3f) */
  poly_small_mul3(f, f);
  poly_small3_add1(f);
  poly_ntt(f, f);
  MAM2_ASSERT(poly_has_inv(f));

  /* NTT(3g) */
  poly_small_mul3(g, g);
  poly_ntt(g, g);
  MAM2_ASSERT(poly_has_inv(g));

  /* NTT(1+x+x^2..)*/
  trits_set1(r, 1);
  poly_small_from_trits(t, r);
  poly_ntt(t, t);

  /* (3g + 1+3f)*(1+x+x^2+..) */
  poly_conv(f, t, h);
  poly_intt(h, s);
  MAM2_ASSERT(3 * 1024 + 1 == poly_coeff_to_trint9(s[511]));
  poly_conv(g, t, h);
  poly_intt(h, h);
  MAM2_ASSERT(3 * 1024 == poly_coeff_to_trint9(h[511]));
  poly_add(h, s, s);
  MAM2_ASSERT(-6 * 1024 == poly_coeff_to_trint9(s[511]));

  return 1;
}

bool_t poly_test_param() {
  bool_t r = 1;
  size_t o, i;
  poly_coeff_t y, yi, w, wi, n, ni, e;

  e = poly_coeff_from_trint9(1);
  r = r && (MAM2_POLY_COEFF_ONE == e);

  y = MAM2_POLY_COEFF_GAMMA;
  o = poly_coeff_order(y, &yi);
  r = r && (o == 2 * MAM2_POLY_N);
  r = r && (yi = MAM2_POLY_COEFF_GAMMA_INV);
  r = r && (MAM2_POLY_COEFF_ONE == poly_coeff_mul(y, yi));

  w = MAM2_POLY_COEFF_OMEGA;
  o = poly_coeff_order(w, &wi);
  r = r && (o == MAM2_POLY_N);
  r = r && (wi == MAM2_POLY_COEFF_OMEGA_INV);
  r = r && (MAM2_POLY_COEFF_ONE == poly_coeff_mul(w, wi));

  n = MAM2_POLY_COEFF_N;
  o = poly_coeff_order(n, &ni);
  r = r && (ni == MAM2_POLY_COEFF_N_INV);
  r = r && (MAM2_POLY_COEFF_ONE == poly_coeff_mul(n, ni));

  for (i = 0; i <= 2 * MAM2_POLY_N; ++i)
    r = r && (MAM2_POLY_COEFF_ONE ==
              poly_coeff_mul(poly_gamma_exp[i],
                             poly_gamma_exp[2 * MAM2_POLY_N - i]));

  return r;
}

bool_t poly_test() {
  bool_t r = 1;

  r = r && poly_test_ntrufail();
  r = r && poly_test_param();

  r = r && poly_test_coeff_mul();
  r = r && poly_test_coeff();
  r = r && poly_test_ntt();
  return r;
}

int main(void) {
  UNITY_BEGIN();

  return UNITY_END();
}
