/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/ntru/poly.h"
#include "mam/v2/ntru/poly_param.h"

#if defined(MAM2_POLY_MRED_BINARY)
poly_coeff_t poly_coeff_mredd(poly_dcoeff_t m) {
  poly_coeff_t spongos;
  spongos = m;

  spongos = (poly_coeff_t)m * MAM2_POLY_MRED_Q_INV;
  /*spongos = (spongos << 12) + (spongos << 13) - spongos;*/

  m = m + spongos * MAM2_POLY_Q;
  /*m = m + (spongos << 12) + (spongos << 13) + spongos;*/

  /* spongos := m div R */
  spongos = m >> MAM2_POLY_MRED_R_LOG;
  spongos = (spongos < MAM2_POLY_Q) ? spongos : (spongos - MAM2_POLY_Q);
  MAM2_ASSERT(spongos < MAM2_POLY_Q);
  return spongos;
}
#endif

/*!
\brief Convert integer into internal polynomial coefficient representation.
\note The input integer must be within the range [-(Q-1)/2,...,(Q-1)/2].
*/
poly_coeff_t poly_coeff_from_trint9(trint9_t t) {
#if defined(MAM2_POLY_MRED_BINARY)
  /* `c*R (mod q)` */
  poly_dcoeff_t d;
  poly_coeff_t c;
  MAM2_ASSERT(-(MAM2_POLY_Q - 1) / 2 <= t && t <= (MAM2_POLY_Q - 1) / 2);

  d = t < 0 ? (poly_dcoeff_t)t + MAM2_POLY_Q : t;
  c = (d << MAM2_POLY_MRED_R_LOG) % MAM2_POLY_Q;
  return c;
#else
  MAM2_ASSERT(-(MAM2_POLY_Q - 1) / 2 <= t && t <= (MAM2_POLY_Q - 1) / 2);
  return (poly_coeff_t)t;
#endif
}

/*!
\brief Convert internal polynomial coefficient representation into an integer.
\note The output integer will be within the range [-(Q-1)/2,...,(Q-1)/2].
*/
trint9_t poly_coeff_to_trint9(poly_coeff_t c) {
#if defined(MAM2_POLY_MRED_BINARY)
  /* `c/R (mods q)` */
  poly_dcoeff_t d = c;
  d *= MAM2_POLY_MRED_RI;
  c = d % (poly_coeff_t)MAM2_POLY_Q;
  trint9_t t = (MAM2_POLY_Q - 1) / 2 < c ? (trint9_t)c - MAM2_POLY_Q : c;
  return t;
#else
  c = (MAM2_POLY_Q - 1) / 2 < c ? c - MAM2_POLY_Q : c;
  return c;
#endif
}

poly_coeff_t poly_coeff_from_trint1(trint1_t t) {
  poly_coeff_t c = 0;
  MAM2_ASSERT_TRINT1(t);
  if (0 < t)
    c = MAM2_POLY_COEFF_ONE;
  else if (t < 0)
    c = MAM2_POLY_COEFF_MINUS_ONE;
  else
    ;
  return c;
}

/*! a + b (mods q) */
poly_coeff_t poly_coeff_add(poly_coeff_t a, poly_coeff_t b) {
#if defined(MAM2_POLY_MRED_BINARY)
  /* u = a + b mod R */
  poly_coeff_t c = a + b;
  c = c < MAM2_POLY_Q ? c : c - MAM2_POLY_Q;
  return c;
#else
  /* `a+b` can overflow, so need double precision */
  poly_dcoeff_t sum = (poly_dcoeff_t)a + b;
  if (sum < -((MAM2_POLY_Q - 1) / 2))
    sum += MAM2_POLY_Q;
  else if (sum > ((MAM2_POLY_Q - 1) / 2))
    sum -= MAM2_POLY_Q;
  else
    ;
  return sum;
#endif
}

/*! a - b (mods q) */
poly_coeff_t poly_coeff_sub(poly_coeff_t a, poly_coeff_t b) {
#if defined(MAM2_POLY_MRED_BINARY)
  /* u = a - b mod R */
  poly_coeff_t c = a < b ? MAM2_POLY_Q + a - b : a - b;
  return c;
#else
  return poly_coeff_add(a, -b);
#endif
}

/*! a * b (mods q) */
poly_coeff_t poly_coeff_mul(poly_coeff_t a, poly_coeff_t b) {
#if defined(MAM2_POLY_MRED_BINARY)
  return poly_coeff_mredd((poly_dcoeff_t)a * b);
#else
  poly_dcoeff_t prod = (poly_dcoeff_t)a * b;
  prod = MAM2_MODS(prod, ((trint18_t)MAM2_POLY_Q * (trint18_t)MAM2_POLY_Q),
                   MAM2_POLY_Q);
  return prod;
#endif
}

/*! a * b + c (mods q) */
poly_coeff_t poly_coeff_mul_add(poly_coeff_t a, poly_coeff_t b,
                                poly_coeff_t c) {
#if defined(MAM2_POLY_MRED_BINARY)
  return poly_coeff_add(poly_coeff_mul(a, b), c);
#else
  poly_dcoeff_t prod = (poly_dcoeff_t)a * b + c;
  prod = MAM2_MODS(prod, ((trint18_t)MAM2_POLY_Q * (trint18_t)MAM2_POLY_Q),
                   MAM2_POLY_Q);
  return prod;
#endif
}

poly_coeff_t poly_coeff_inv(poly_coeff_t a) {
  poly_coeff_t e, t;
  size_t i;
  MAM2_ASSERT(MAM2_POLY_Q == ((3 * (1 << 12)) + 1));
  /* q-2 = 10111111111111b */

  e = a;
  for (i = 0; i < 11; ++i) {
    t = poly_coeff_mul(e, e);
    e = poly_coeff_mul(t, a);
  }

  t = poly_coeff_mul(e, a);
  t = poly_coeff_mul(t, t);
  e = poly_coeff_mul(t, e);

  MAM2_ASSERT(poly_coeff_mul(e, a) == MAM2_POLY_COEFF_ONE);
  return e;
}

void poly_small3_add1(poly_t f) {
#if !defined(MAM2_POLY_MRED_BINARY)
  MAM2_ASSERT(-3 <= f[0] && f[0] <= 3);
#endif
  f[0] += MAM2_POLY_COEFF_ONE;
}

void poly_small_mul3(poly_t f, poly_t h) {
  size_t i;
  poly_coeff_t c;
  for (i = 0; i < MAM2_POLY_N; ++i) {
    c = poly_coeff_add(f[i], f[i]);
    h[i] = poly_coeff_add(c, f[i]);
  }
}

void poly_add(poly_t f, poly_t g, poly_t h) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i) h[i] = poly_coeff_add(f[i], g[i]);
}

void poly_sub(poly_t f, poly_t g, poly_t h) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i) h[i] = poly_coeff_sub(f[i], g[i]);
}

void poly_conv(poly_t tf, poly_t tg, poly_t th) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i) th[i] = poly_coeff_mul(tf[i], tg[i]);
}

bool poly_has_inv(poly_t t) {
  bool r = true;
  size_t i;
  for (i = 0; r && i < MAM2_POLY_N; ++i) r = (0 != t[i]) ? 1 : 0;
  return r;
}

void poly_inv(poly_t tf, poly_t th) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i) th[i] = poly_coeff_inv(tf[i]);
}

/* \brief tⱼ = f(γ²ʲ⁺¹) ≡ Σᵢfᵢγ⁽²ʲ⁺¹⁾ⁱ */
void poly_ntt(poly_t f, poly_t t) {
  poly_t u;
  poly_coeff_t c, d;
  size_t i, j, r;

  for (i = 0; i < MAM2_POLY_N; ++i)
    u[poly_idx_rev[i]] = poly_coeff_mul(poly_gamma_exp[i], f[i]);

  for (i = MAM2_POLY_N_LOG; i--;) {
    for (j = 0; j < MAM2_POLY_N / 2; ++j) {
      r = j & ~(((size_t)1 << i) - 1);
      /*r = j - (j % (1 << i));*/
      c = u[j + j];
      d = poly_coeff_mul(u[j + j + 1], poly_gamma_exp[r + r]);
      t[j] = poly_coeff_add(c, d);
      t[j + MAM2_POLY_N / 2] = poly_coeff_sub(c, d);
    }
    for (j = 0; j < MAM2_POLY_N; ++j) u[j] = t[j];
  }
}

/*
\brief fₖ = γ⁻ᵏn⁻¹ t(γ⁻²ᵏ)
\note
t(γ⁻²ᵏ)
 ≡ Σⱼtⱼγ⁻²ᵏʲ
 ≡ Σⱼ(Σᵢfᵢγ⁽²ʲ⁺¹⁾ⁱ)γ⁻²ᵏʲ
 ≡ Σᵢfᵢ(Σⱼγ⁽²ʲ⁺¹⁾ⁱ⁻²ᵏʲ)
 ≡ Σᵢfᵢγⁱ(Σⱼγ²ʲ⁽ⁱ⁻ᵏ⁾)
 ≡ fₖγᵏn
*/
void poly_intt(poly_t t, poly_t f) {
  poly_t u;
  poly_coeff_t c, d;
  size_t i, j, r;

  for (i = 0; i < MAM2_POLY_N; ++i) u[poly_idx_rev[i]] = t[i];

  for (i = MAM2_POLY_N_LOG; i--;) {
    for (j = 0; j < MAM2_POLY_N / 2; ++j) {
      r = j & ~(((size_t)1 << i) - 1);
      /*r = j - (j % (1 << i));*/
      c = u[j + j];
      d = poly_coeff_mul(u[j + j + 1],
                         poly_gamma_exp[2 * MAM2_POLY_N - (r + r)]);
      f[j] = poly_coeff_add(c, d);
      f[j + MAM2_POLY_N / 2] = poly_coeff_sub(c, d);
    }
    for (j = 0; j < MAM2_POLY_N; ++j) u[j] = f[j];
  }

  for (i = 0; i < MAM2_POLY_N; ++i) {
    /* TODO: precomp γ⁻ⁱn⁻¹? */
    c = poly_coeff_mul(MAM2_POLY_COEFF_N_INV,
                       poly_gamma_exp[2 * MAM2_POLY_N - i]);
    f[i] = poly_coeff_mul(c, f[i]);
  }
}

void poly_round_to_trits(poly_t f, trits_t t) {
  size_t i;
  trint9_t c;
  MAM2_ASSERT(trits_size(t) == MAM2_POLY_N);

  for (i = 0; i != MAM2_POLY_N; ++i, t = trits_drop(t, 1)) {
    c = poly_coeff_to_trint9(f[i]);
    c = MAM2_MODS(c, 27 * 27 * 27, 3);
    trits_put1(t, (trit_t)c);
  }
}

void poly_small_from_trits(poly_t f, trits_t t) {
  size_t i;
  MAM2_ASSERT(trits_size(t) == MAM2_POLY_N);

  for (i = 0; i < MAM2_POLY_N; ++i, t = trits_drop(t, 1))
    f[i] = poly_coeff_from_trint1(trits_get1(t));
}

bool poly_from_trits(poly_t f, trits_t t) {
  size_t i;
  trint9_t c;
  MAM2_ASSERT(trits_size(t) == 9 * MAM2_POLY_N);

  for (i = 0; i < MAM2_POLY_N; ++i, t = trits_drop(t, 9)) {
    c = trits_get9(t);
    if ((c < -((MAM2_POLY_Q - 1) / 2)) || (((MAM2_POLY_Q - 1) / 2) < c)) break;
    f[i] = poly_coeff_from_trint9(c);
  }
  return (i == MAM2_POLY_N) ? 1 : 0;
}

void poly_to_trits(poly_t f, trits_t t) {
  size_t i;
  MAM2_ASSERT(trits_size(t) == 9 * MAM2_POLY_N);

  for (i = 0; i < MAM2_POLY_N; ++i, t = trits_drop(t, 9))
    trits_put9(t, poly_coeff_to_trint9(f[i]));
}

poly_coeff_t poly_eval(poly_t f, poly_coeff_t x) {
  poly_coeff_t r = 0;
  size_t i;
  for (i = MAM2_POLY_N; i--;) r = poly_coeff_mul_add(x, r, f[i]);
  return r;
}
