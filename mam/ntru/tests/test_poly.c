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

#include "mam/ntru/poly.h"
#include "mam/ntru/poly_param.h"

static bool poly_is_one(poly_t h) {
  size_t i;
  for (i = 1; i < MAM_POLY_N; ++i)
    if (h[i] != 0) break;
  return (MAM_POLY_COEFF_ONE == h[0]) && (i == MAM_POLY_N) ? 1 : 0;
}

static bool poly_is_eq(poly_t f, poly_t g) {
  bool r = true;
  size_t i;
  for (i = 0; r && i < MAM_POLY_N; ++i) r = (f[i] == g[i]) ? 1 : 0;
  return r;
}

poly_coeff_t poly_eval(poly_t f, poly_coeff_t x) {
  poly_coeff_t r = 0;
  size_t i;
  for (i = MAM_POLY_N; i--;) r = poly_coeff_mul_add(x, r, f[i]);
  return r;
}

static void poly_mul(poly_t f, poly_t g, poly_t h) {
  typedef trint9_t poly2_t[2 * MAM_POLY_N - 1];
  poly2_t fg;
  size_t i, j;

  for (i = 0; i < 2 * MAM_POLY_N - 1; ++i) fg[i] = 0;

  for (i = 0; i < MAM_POLY_N; ++i)
    for (j = 0; j < MAM_POLY_N; ++j) fg[i + j] = poly_coeff_mul_add(f[i], g[j], fg[i + j]);

  for (i = 0; i < MAM_POLY_N - 1; ++i) h[i] = poly_coeff_sub(fg[i], fg[i + MAM_POLY_N]);
  h[i] = fg[i];
}

static void poly_ntt2(poly_t f, poly_t t) {
  size_t i;
  for (i = 0; i < MAM_POLY_N; ++i) t[i] = poly_eval(f, poly_gamma_exp[i + i + 1]);
}

static void poly_intt2(poly_t t, poly_t f) {
  size_t i;
  for (i = 0; i < MAM_POLY_N; ++i)
    f[i] = poly_coeff_mul(poly_coeff_mul(MAM_POLY_COEFF_N_INV, poly_gamma_exp[2 * MAM_POLY_N - i]),
                          poly_eval(t, poly_gamma_exp[2 * MAM_POLY_N - (i + i)]));
}

static size_t poly_coeff_order(poly_coeff_t u, poly_coeff_t *ui) {
  size_t i;
  poly_coeff_t v;

  *ui = MAM_POLY_COEFF_ONE;
  for (i = 0; i++ < MAM_POLY_Q; *ui = v)
    if (MAM_POLY_COEFF_ONE == (v = poly_coeff_mul(*ui, u))) /*MAM_ASSERT(poly_coeff_inv(u) == *ui);*/
      return i;

  return 0;
}

static void poly_ntt_test(void) {
  size_t i, k, ki;
  poly_t f, g, fg, tf, tg, tfg, itf, itg, itfg, tif, itif, fitif, t2, it2;
  poly_coeff_t c = 7, a = 13, two = 1;

  for (i = 0; i < MAM_POLY_N; ++i, two = poly_coeff_mul(two, 2))
    f[i] = 0,     /*two,*/
        g[i] = 0; /*poly_coeff_add(two, 1233);*/

  f[0] = MAM_POLY_COEFF_ONE; /* f(x) = 1 */
  g[1] = MAM_POLY_COEFF_ONE; /* g(x) = x */

  for (k = ki = 0; k < 100; ++k) {
    poly_ntt2(f, t2);
    poly_intt2(t2, it2);
    poly_ntt(f, tf);
    poly_intt(tf, itf);
    TEST_ASSERT_TRUE(poly_is_eq(f, itf));

    poly_ntt2(g, t2);
    poly_intt2(t2, it2);
    poly_ntt(g, tg);
    poly_intt(tg, itg);
    TEST_ASSERT_TRUE(poly_is_eq(g, itg));

    poly_mul(f, g, fg);
    poly_ntt(g, tg);
    poly_conv(tf, tg, tfg);
    poly_intt(tfg, itfg);
    TEST_ASSERT_TRUE(poly_is_eq(fg, itfg));

    if (poly_has_inv(tf)) {
      ++ki;
      poly_inv(tf, tif);
      poly_intt(tif, itif);
      poly_mul(f, itif, fitif);
      TEST_ASSERT_TRUE(poly_is_one(fitif));
    }

    for (i = 0; i < MAM_POLY_N; ++i) f[i] = poly_coeff_add(a, poly_coeff_mul(c, f[i]));
  }
}

static void poly_coeff_mul_test(void) {
  size_t i, j;
  poly_coeff_t a, b, m;
  trint9_t t, n;

  for (i = 0; i <= MAM_POLY_Q / 2; ++i)
    for (j = 0; j <= MAM_POLY_Q / 2; ++j) {
      a = poly_coeff_from_trint9((trint9_t)i);
      b = poly_coeff_from_trint9((trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM_POLY_Q;
      t = t <= MAM_POLY_Q / 2 ? t : t - MAM_POLY_Q;
      TEST_ASSERT_TRUE(n == t);

      a = poly_coeff_from_trint9(-(trint9_t)i);
      b = poly_coeff_from_trint9((trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM_POLY_Q;
      t = t <= MAM_POLY_Q / 2 ? -t : MAM_POLY_Q - t;
      TEST_ASSERT_TRUE(n == t);

      a = poly_coeff_from_trint9((trint9_t)i);
      b = poly_coeff_from_trint9(-(trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM_POLY_Q;
      t = t <= MAM_POLY_Q / 2 ? -t : MAM_POLY_Q - t;
      TEST_ASSERT_TRUE(n == t);

      a = poly_coeff_from_trint9(-(trint9_t)i);
      b = poly_coeff_from_trint9(-(trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM_POLY_Q;
      t = t <= MAM_POLY_Q / 2 ? t : t - MAM_POLY_Q;
      TEST_ASSERT_TRUE(n == t);
    }
}

static void poly_coeff_test(void) {
  size_t i;
#define MAM_POLY_TEST_N 201
  trint9_t tt[MAM_POLY_TEST_N];

  size_t ia, ib, ic;
  poly_coeff_t a, b, c, m;
  trint9_t t;
  trint18_t s;

  for (i = 0; i < MAM_POLY_TEST_N / 4; ++i) tt[i] = (trint9_t)(-(MAM_POLY_Q - 1) / 2 + i);
  for (; i < MAM_POLY_TEST_N / 2; ++i) tt[i] = (trint9_t)(MAM_POLY_TEST_N / 2 - i);
  tt[i++] = 0;
  for (; i < MAM_POLY_TEST_N; ++i) tt[i] = -tt[MAM_POLY_TEST_N - 1 - i];

  for (ia = 0; ia < MAM_POLY_TEST_N; ++ia) {
    a = poly_coeff_from_trint9(tt[ia]);
    t = poly_coeff_to_trint9(a);
    TEST_ASSERT_TRUE(tt[ia] == t);

    for (ib = 0; ib < MAM_POLY_TEST_N; ++ib) {
      b = poly_coeff_from_trint9(tt[ib]);

      m = poly_coeff_add(a, b);
      t = poly_coeff_to_trint9(m);
      s = (trint18_t)tt[ia] + tt[ib];
      s = MAM_MODS(s, (MAM_POLY_Q * MAM_POLY_Q), MAM_POLY_Q);
      TEST_ASSERT_TRUE(t == s);

      m = poly_coeff_sub(a, b);
      t = poly_coeff_to_trint9(m);
      s = (trint18_t)tt[ia] - tt[ib];
      s = MAM_MODS(s, (MAM_POLY_Q * MAM_POLY_Q), MAM_POLY_Q);
      TEST_ASSERT_TRUE(t == s);

      m = poly_coeff_mul(a, b);
      t = poly_coeff_to_trint9(m);
      s = (trint18_t)tt[ia] * tt[ib];
      s = MAM_MODS(s, (MAM_POLY_Q * MAM_POLY_Q), MAM_POLY_Q);
      TEST_ASSERT_TRUE(t == s);

      for (ic = 0; ic < MAM_POLY_TEST_N; ++ic) {
        c = poly_coeff_from_trint9(tt[ic]);
        m = poly_coeff_mul_add(a, b, c);
        t = poly_coeff_to_trint9(m);
        s = (trint18_t)tt[ia] * tt[ib] + tt[ic];
        s = MAM_MODS(s, (MAM_POLY_Q * MAM_POLY_Q), MAM_POLY_Q);
        TEST_ASSERT_TRUE(t == s);
      }
    }
  }
#undef MAM_POLY_TEST_N
}

static void poly_ntrufail_test(void) {
  MAM_POLY_DEF(f);
  MAM_POLY_DEF(g);
  MAM_POLY_DEF(h);
  MAM_POLY_DEF(t);
  MAM_POLY_DEF(s);
  MAM_TRITS_DEF(r, 1024);
  r = MAM_TRITS_INIT(r, 1024);

  trits_set1(r, 1);
  trits_set1(trits_drop(r, 512), -1);
  poly_small_from_trits(f, r);
  poly_small_from_trits(g, r);

  /* NTT(1+3f) */
  poly_small_mul3(f, f);
  poly_small3_add1(f);
  poly_ntt(f, f);
  TEST_ASSERT_TRUE(poly_has_inv(f));

  /* NTT(3g) */
  poly_small_mul3(g, g);
  poly_ntt(g, g);
  TEST_ASSERT_TRUE(poly_has_inv(g));

  /* NTT(1+x+x^2..)*/
  trits_set1(r, 1);
  poly_small_from_trits(t, r);
  poly_ntt(t, t);

  /* (3g + 1+3f)*(1+x+x^2+..) */
  poly_conv(f, t, h);
  poly_intt(h, s);
  TEST_ASSERT_TRUE(3 * 1024 + 1 == poly_coeff_to_trint9(s[511]));
  poly_conv(g, t, h);
  poly_intt(h, h);
  TEST_ASSERT_TRUE(3 * 1024 == poly_coeff_to_trint9(h[511]));
  poly_add(h, s, s);
  TEST_ASSERT_TRUE(-6 * 1024 == poly_coeff_to_trint9(s[511]));
}

static void poly_param_test(void) {
  size_t o, i;
  poly_coeff_t y, yi, w, wi, n, ni, e;

  e = poly_coeff_from_trint9(1);
  TEST_ASSERT_TRUE(MAM_POLY_COEFF_ONE == e);

  y = MAM_POLY_COEFF_GAMMA;
  o = poly_coeff_order(y, &yi);
  TEST_ASSERT_TRUE(o == 2 * MAM_POLY_N);
  TEST_ASSERT_TRUE(yi = MAM_POLY_COEFF_GAMMA_INV);
  TEST_ASSERT_TRUE(MAM_POLY_COEFF_ONE == poly_coeff_mul(y, yi));

  w = MAM_POLY_COEFF_OMEGA;
  o = poly_coeff_order(w, &wi);
  TEST_ASSERT_TRUE(o == MAM_POLY_N);
  TEST_ASSERT_TRUE(wi == MAM_POLY_COEFF_OMEGA_INV);
  TEST_ASSERT_TRUE(MAM_POLY_COEFF_ONE == poly_coeff_mul(w, wi));

  n = MAM_POLY_COEFF_N;
  o = poly_coeff_order(n, &ni);
  TEST_ASSERT_TRUE(ni == MAM_POLY_COEFF_N_INV);
  TEST_ASSERT_TRUE(MAM_POLY_COEFF_ONE == poly_coeff_mul(n, ni));

  for (i = 0; i <= 2 * MAM_POLY_N; ++i)
    TEST_ASSERT_TRUE(MAM_POLY_COEFF_ONE == poly_coeff_mul(poly_gamma_exp[i], poly_gamma_exp[2 * MAM_POLY_N - i]));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(poly_ntrufail_test);
  RUN_TEST(poly_param_test);
  RUN_TEST(poly_coeff_mul_test);
  RUN_TEST(poly_coeff_test);
  RUN_TEST(poly_ntt_test);

  return UNITY_END();
}
