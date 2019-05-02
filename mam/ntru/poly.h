/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup mam
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef __MAM_NTRU_POLY_H__
#define __MAM_NTRU_POLY_H__

#include "mam/defs.h"
#include "mam/trits/trits.h"

// Prime modulus q = 3*2^12+1
// Some algorithms are specifically crafted for this value
#define MAM_POLY_Q 12289

// Degree `n` of polynomial `m(x) = (x^n+1)`
#define MAM_POLY_N_LOG 10
#define MAM_POLY_N (1 << MAM_POLY_N_LOG)

/**
 * γ²ⁿ ≡ γ γ⁻¹ ≡ 1 (mod q)
 * ωⁿ  ≡ ω ω⁻¹ ≡ 1 (mod q). ω ≡ γ² (mod q)
 * n n⁻¹ ≡ 1 (mod q)
 */
#if defined(MAM_POLY_MRED_BINARY)

#define MAM_POLY_COEFF_GAMMA ((poly_coeff_t)4059)
#define MAM_POLY_COEFF_GAMMA_INV ((poly_coeff_t)2340)
#define MAM_POLY_COEFF_OMEGA ((poly_coeff_t)3835)
#define MAM_POLY_COEFF_OMEGA_INV ((poly_coeff_t)5601)
#define MAM_POLY_COEFF_N ((poly_coeff_t)10924)
#define MAM_POLY_COEFF_N_INV ((poly_coeff_t)64)
#define MAM_POLY_COEFF_ONE ((poly_coeff_t)4091)
#define MAM_POLY_COEFF_MINUS_ONE ((poly_coeff_t)MAM_POLY_Q - MAM_POLY_COEFF_ONE)
// R = 2¹⁶
#define MAM_POLY_MRED_R_LOG 16
// R⁻¹ = 9 2⁸ ≡ 2304 (mod q)
#define MAM_POLY_MRED_RI 2304
// q⁻¹ q - R R⁻¹ = 1
#define MAM_POLY_MRED_Q_INV 12287

#else

#define MAM_POLY_COEFF_GAMMA ((poly_coeff_t)7)
#define MAM_POLY_COEFF_GAMMA_INV ((poly_coeff_t)-3511)
#define MAM_POLY_COEFF_OMEGA ((poly_coeff_t)49)
#define MAM_POLY_COEFF_OMEGA_INV ((poly_coeff_t)1254)
#define MAM_POLY_COEFF_N ((poly_coeff_t)1024)
#define MAM_POLY_COEFF_N_INV ((poly_coeff_t)-12)
#define MAM_POLY_COEFF_ONE ((poly_coeff_t)1)
#define MAM_POLY_COEFF_MINUS_ONE ((poly_coeff_t)-1)

#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type of polynomial coefficients mods (q)
 * Depending on configuration, coefficients may be represented in Montgomery
 * reduced form
 */
#if defined(MAM_POLY_MRED_BINARY)
typedef uint16_t poly_coeff_t;
typedef uint32_t poly_dcoeff_t;
#else
typedef trint9_t poly_coeff_t;
typedef trint18_t poly_dcoeff_t;
#endif

// Type of polynomials mods (x^n+1, q)
typedef poly_coeff_t poly_t[MAM_POLY_N];
#define MAM_POLY_DEF(f) poly_t f

#if defined(MAM_POLY_MRED_BINARY)
poly_coeff_t poly_coeff_mredd(poly_dcoeff_t m);
#endif

static inline poly_coeff_t poly_coeff_from_trint1(trint1_t const t) {
  MAM_ASSERT_TRINT1(t);

  if (t > 0)
    return MAM_POLY_COEFF_ONE;
  else if (t < 0)
    return MAM_POLY_COEFF_MINUS_ONE;
  return 0;
}

/**
 * Convert integer into internal polynomial coefficient representation
 * The input integer must be within the range [-(Q-1)/2,...,(Q-1)/2]
 */
poly_coeff_t poly_coeff_from_trint9(trint9_t const t);

/**
 * Convert internal polynomial coefficient representation into an integer
 * The output integer will be within the range [-(Q-1)/2,...,(Q-1)/2]
 */
trint9_t poly_coeff_to_trint9(poly_coeff_t c);

// a + b (mods q)
poly_coeff_t poly_coeff_add(poly_coeff_t const a, poly_coeff_t const b);

// a - b (mods q)
static inline poly_coeff_t poly_coeff_sub(poly_coeff_t const a, poly_coeff_t const b) {
#if defined(MAM_POLY_MRED_BINARY)
  /* u = a - b mod R */
  poly_coeff_t c = a < b ? MAM_POLY_Q + a - b : a - b;
  return c;
#else
  return poly_coeff_add(a, -b);
#endif
}

// a * b (mods q)
static inline poly_coeff_t poly_coeff_mul(poly_coeff_t const a, poly_coeff_t const b) {
#if defined(MAM_POLY_MRED_BINARY)
  return poly_coeff_mredd((poly_dcoeff_t)a * b);
#else
  poly_dcoeff_t prod = (poly_dcoeff_t)a * b;

  prod = MAM_MODS(prod, ((trint18_t)MAM_POLY_Q * (trint18_t)MAM_POLY_Q), MAM_POLY_Q);

  return prod;
#endif
}

// a * b + c (mods q)
static inline poly_coeff_t poly_coeff_mul_add(poly_coeff_t const a, poly_coeff_t const b, poly_coeff_t const c) {
#if defined(MAM_POLY_MRED_BINARY)
  return poly_coeff_add(poly_coeff_mul(a, b), c);
#else
  poly_dcoeff_t prod = (poly_dcoeff_t)a * b + c;

  prod = MAM_MODS(prod, ((trint18_t)MAM_POLY_Q * (trint18_t)MAM_POLY_Q), MAM_POLY_Q);

  return prod;
#endif
}

poly_coeff_t poly_coeff_inv(poly_coeff_t const a);

// f(x) := f(x) + 1; f(x)/3 is small
static inline void poly_small3_add1(poly_t f) {
#if !defined(MAM_POLY_MRED_BINARY)
  MAM_ASSERT(-3 <= f[0] && f[0] <= 3);
#endif
  f[0] += MAM_POLY_COEFF_ONE;
}

// h(x) := 3 f(x); f(x) is small
static inline void poly_small_mul3(poly_t const f, poly_t h) {
  poly_coeff_t c;

  for (size_t i = 0; i < MAM_POLY_N; ++i) {
    c = poly_coeff_add(f[i], f[i]);
    h[i] = poly_coeff_add(c, f[i]);
  }
}

// h(x) := f(x) + g(x) mods (m(x), q)
static inline void poly_add(poly_t const f, poly_t const g, poly_t h) {
  for (size_t i = 0; i < MAM_POLY_N; ++i) {
    h[i] = poly_coeff_add(f[i], g[i]);
  }
}

// h(x) := f(x) - g(x) mods (m(x), q)
static inline void poly_sub(poly_t const f, poly_t const g, poly_t h) {
  for (size_t i = 0; i < MAM_POLY_N; ++i) {
    h[i] = poly_coeff_sub(f[i], g[i]);
  }
}

/**
 * th := NTT(f) ⊛ NTT(g) mods (m(x), q)
 * h(x) ≡ f(x) * g(x) mods (m(x), q)
 */
static inline void poly_conv(poly_t const tf, poly_t const tg, poly_t th) {
  for (size_t i = 0; i < MAM_POLY_N; ++i) {
    th[i] = poly_coeff_mul(tf[i], tg[i]);
  }
}

// ∃? h(x) : 1 ≡ f(x) * h(x) mods (m(x), q), t = NTT(f)
static inline bool poly_has_inv(poly_t const t) {
  bool r = true;

  for (size_t i = 0; r && i < MAM_POLY_N; ++i) {
    r = (0 != t[i]) ? 1 : 0;
  }

  return r;
}

// h(x) := f⁻¹(x) mods (m(x), q), tf = NTT(f), th = NTT(h)
static inline void poly_inv(poly_t const tf, poly_t th) {
  for (size_t i = 0; i < MAM_POLY_N; ++i) {
    th[i] = poly_coeff_inv(tf[i]);
  }
}

/**
 * t(x) := NTT(f)
 * tⱼ = f(γ²ʲ⁺¹) ≡ Σᵢfᵢγ⁽²ʲ⁺¹⁾ⁱ
 */
void poly_ntt(poly_t const f, poly_t t);

/**
 * f(x) := NTT⁻¹(t)
 * fₖ = γ⁻ᵏn⁻¹ t(γ⁻²ᵏ)
 *
 * t(γ⁻²ᵏ)
 * ≡ Σⱼtⱼγ⁻²ᵏʲ
 * ≡ Σⱼ(Σᵢfᵢγ⁽²ʲ⁺¹⁾ⁱ)γ⁻²ᵏʲ
 * ≡ Σᵢfᵢ(Σⱼγ⁽²ʲ⁺¹⁾ⁱ⁻²ᵏʲ)
 * ≡ Σᵢfᵢγⁱ(Σⱼγ²ʲ⁽ⁱ⁻ᵏ⁾)
 * ≡ fₖγᵏn
 */
void poly_intt(poly_t const t, poly_t f);

// tᵢ := fᵢ (mods 3)
void poly_round_to_trits(poly_t const f, trits_t t);

// fᵢ := tᵢ
static inline void poly_small_from_trits(poly_t f, trits_t t) {
  MAM_ASSERT(trits_size(t) == MAM_POLY_N);

  for (size_t i = 0; i < MAM_POLY_N; ++i, t = trits_drop(t, 1)) {
    f[i] = poly_coeff_from_trint1(trits_get1(t));
  }
}

// fᵢ := tᵢ
bool poly_from_trits(poly_t f, trits_t t);

// tᵢ := fᵢ ???
static inline void poly_to_trits(poly_t f, trits_t t) {
  MAM_ASSERT(trits_size(t) == 9 * MAM_POLY_N);

  for (size_t i = 0; i < MAM_POLY_N; ++i, t = trits_drop(t, 9)) {
    trits_put9(t, poly_coeff_to_trint9(f[i]));
  }
}

#ifdef __cplusplus
}
#endif

#endif  // __MAM_NTRU_POLY_H__

/** @} */
