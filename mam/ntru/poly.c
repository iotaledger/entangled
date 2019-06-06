/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/ntru/poly.h"
#include "mam/ntru/poly_param.h"

/*
 * Public functions
 */

#if defined(MAM_POLY_MRED_BINARY)
poly_coeff_t poly_coeff_mredd(poly_dcoeff_t m) {
  poly_coeff_t s = m;

  s = (poly_coeff_t)m * MAM_POLY_MRED_Q_INV;
  /*s = (s << 12) + (s << 13) - s;*/

  m = m + s * MAM_POLY_Q;
  /*m = m + (s << 12) + (s << 13) + s;*/

  /* s := m div R */
  s = m >> MAM_POLY_MRED_R_LOG;
  s = (s < MAM_POLY_Q) ? s : (s - MAM_POLY_Q);
  MAM_ASSERT(s < MAM_POLY_Q);

  return s;
}
#endif

void poly_ntt(poly_t const f, poly_t t) {
  poly_t u;
  poly_coeff_t c, d;
  size_t r;

  for (size_t i = 0; i < MAM_POLY_N; ++i) {
    u[poly_idx_rev[i]] = poly_coeff_mul(poly_gamma_exp[i], f[i]);
  }

  for (size_t i = MAM_POLY_N_LOG; i--;) {
    for (size_t j = 0; j < MAM_POLY_N / 2; ++j) {
      r = j & ~(((size_t)1 << i) - 1);
      /*r = j - (j % (1 << i));*/
      c = u[j + j];
      d = poly_coeff_mul(u[j + j + 1], poly_gamma_exp[r + r]);
      t[j] = poly_coeff_add(c, d);
      t[j + MAM_POLY_N / 2] = poly_coeff_sub(c, d);
    }
    for (size_t j = 0; j < MAM_POLY_N; ++j) {
      u[j] = t[j];
    }
  }
}

void poly_intt(poly_t const t, poly_t f) {
  poly_t u;
  poly_coeff_t c, d;
  size_t r;

  for (size_t i = 0; i < MAM_POLY_N; ++i) {
    u[poly_idx_rev[i]] = t[i];
  }

  for (size_t i = MAM_POLY_N_LOG; i--;) {
    for (size_t j = 0; j < MAM_POLY_N / 2; ++j) {
      r = j & ~(((size_t)1 << i) - 1);
      /*r = j - (j % (1 << i));*/
      c = u[j + j];
      d = poly_coeff_mul(u[j + j + 1], poly_gamma_exp[2 * MAM_POLY_N - (r + r)]);
      f[j] = poly_coeff_add(c, d);
      f[j + MAM_POLY_N / 2] = poly_coeff_sub(c, d);
    }
    for (size_t j = 0; j < MAM_POLY_N; ++j) {
      u[j] = f[j];
    }
  }

  for (size_t i = 0; i < MAM_POLY_N; ++i) {
    /* TODO: precomp γ⁻ⁱn⁻¹? */
    c = poly_coeff_mul(MAM_POLY_COEFF_N_INV, poly_gamma_exp[2 * MAM_POLY_N - i]);
    f[i] = poly_coeff_mul(c, f[i]);
  }
}

void poly_round_to_trits(poly_t const f, trits_t t) {
  trint9_t c;
  MAM_ASSERT(trits_size(t) == MAM_POLY_N);

  for (size_t i = 0; i != MAM_POLY_N; ++i, t = trits_drop(t, 1)) {
    c = poly_coeff_to_trint9(f[i]);
    c = MAM_MODS(c, 27 * 27 * 27, 3);
    trits_put1(t, (trit_t)c);
  }
}

bool poly_from_trits(poly_t f, trits_t t) {
  size_t i;
  trint9_t c;
  MAM_ASSERT(trits_size(t) == 9 * MAM_POLY_N);

  for (i = 0; i < MAM_POLY_N; ++i, t = trits_drop(t, 9)) {
    c = trits_get9(t);
    if ((c < -((MAM_POLY_Q - 1) / 2)) || (((MAM_POLY_Q - 1) / 2) < c)) {
      break;
    }
    f[i] = poly_coeff_from_trint9(c);
  }
  return (i == MAM_POLY_N) ? 1 : 0;
}

poly_coeff_t poly_coeff_from_trint9(trint9_t const t) {
#if defined(MAM_POLY_MRED_BINARY)
  /* `c*R (mod q)` */
  poly_dcoeff_t d;
  poly_coeff_t c;
  MAM_ASSERT(-(MAM_POLY_Q - 1) / 2 <= t && t <= (MAM_POLY_Q - 1) / 2);

  d = t < 0 ? (poly_dcoeff_t)t + MAM_POLY_Q : (poly_dcoeff_t)t;
  c = (d << MAM_POLY_MRED_R_LOG) % MAM_POLY_Q;

  return c;
#else
  MAM_ASSERT(-(MAM_POLY_Q - 1) / 2 <= t && t <= (MAM_POLY_Q - 1) / 2);

  return (poly_coeff_t)t;
#endif
}

trint9_t poly_coeff_to_trint9(poly_coeff_t c) {
#if defined(MAM_POLY_MRED_BINARY)
  /* `c/R (mods q)` */
  poly_dcoeff_t d = c;
  trint9_t t;

  d *= MAM_POLY_MRED_RI;
  c = d % (poly_coeff_t)MAM_POLY_Q;
  t = (MAM_POLY_Q - 1) / 2 < c ? (trint9_t)c - MAM_POLY_Q : c;

  return t;
#else
  c = (MAM_POLY_Q - 1) / 2 < c ? c - MAM_POLY_Q : c;

  return c;
#endif
}

poly_coeff_t poly_coeff_add(poly_coeff_t const a, poly_coeff_t const b) {
#if defined(MAM_POLY_MRED_BINARY)
  /* u = a + b mod R */
  poly_coeff_t c = a + b;

  c = c < MAM_POLY_Q ? c : c - MAM_POLY_Q;

  return c;
#else
  /* `a+b` can overflow, so need double precision */
  poly_dcoeff_t sum = (poly_dcoeff_t)a + b;

  if (sum < -((MAM_POLY_Q - 1) / 2))
    sum += MAM_POLY_Q;
  else if (sum > ((MAM_POLY_Q - 1) / 2))
    sum -= MAM_POLY_Q;

  return sum;
#endif
}

poly_coeff_t poly_coeff_inv(poly_coeff_t const a) {
  poly_coeff_t e, t;

  MAM_ASSERT(MAM_POLY_Q == ((3 * (1 << 12)) + 1));
  /* q-2 = 10111111111111b */

  e = a;
  for (size_t i = 0; i < 11; ++i) {
    t = poly_coeff_mul(e, e);
    e = poly_coeff_mul(t, a);
  }

  t = poly_coeff_mul(e, a);
  t = poly_coeff_mul(t, t);
  e = poly_coeff_mul(t, e);

  MAM_ASSERT(poly_coeff_mul(e, a) == MAM_POLY_COEFF_ONE);

  return e;
}
