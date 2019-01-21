/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_NTRU_POLY_H__
#define __MAM_V2_NTRU_POLY_H__

#include "mam/v2/defs.h"
#include "mam/v2/trits/trits.h"

// Prime modulus q = 3*2^12+1
// Some algorithms are specifically crafted for this value
#define MAM2_POLY_Q 12289

// Degree `n` of polynomial `m(x) = (x^n+1)`
#define MAM2_POLY_N_LOG 10
#define MAM2_POLY_N (1 << MAM2_POLY_N_LOG)

/**
 * γ²ⁿ ≡ γ γ⁻¹ ≡ 1 (mod q)
 * ωⁿ  ≡ ω ω⁻¹ ≡ 1 (mod q). ω ≡ γ² (mod q)
 * n n⁻¹ ≡ 1 (mod q)
 */
#if defined(MAM2_POLY_MRED_BINARY)

#define MAM2_POLY_COEFF_GAMMA ((poly_coeff_t)4059)
#define MAM2_POLY_COEFF_GAMMA_INV ((poly_coeff_t)2340)
#define MAM2_POLY_COEFF_OMEGA ((poly_coeff_t)3835)
#define MAM2_POLY_COEFF_OMEGA_INV ((poly_coeff_t)5601)
#define MAM2_POLY_COEFF_N ((poly_coeff_t)10924)
#define MAM2_POLY_COEFF_N_INV ((poly_coeff_t)64)
#define MAM2_POLY_COEFF_ONE ((poly_coeff_t)4091)
#define MAM2_POLY_COEFF_MINUS_ONE \
  ((poly_coeff_t)MAM2_POLY_Q - MAM2_POLY_COEFF_ONE)
// R = 2¹⁶
#define MAM2_POLY_MRED_R_LOG 16
// R⁻¹ = 9 2⁸ ≡ 2304 (mod q)
#define MAM2_POLY_MRED_RI 2304
// q⁻¹ q - R R⁻¹ = 1
#define MAM2_POLY_MRED_Q_INV 12287

#else

#define MAM2_POLY_COEFF_GAMMA ((poly_coeff_t)7)
#define MAM2_POLY_COEFF_GAMMA_INV ((poly_coeff_t)-3511)
#define MAM2_POLY_COEFF_OMEGA ((poly_coeff_t)49)
#define MAM2_POLY_COEFF_OMEGA_INV ((poly_coeff_t)1254)
#define MAM2_POLY_COEFF_N ((poly_coeff_t)1024)
#define MAM2_POLY_COEFF_N_INV ((poly_coeff_t)-12)
#define MAM2_POLY_COEFF_ONE ((poly_coeff_t)1)
#define MAM2_POLY_COEFF_MINUS_ONE ((poly_coeff_t)-1)

#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type of polynomial coefficients mods (q)
 * Depending on configuration, coefficients may be represented in Montgomery
 * reduced form
 */
#if defined(MAM2_POLY_MRED_BINARY)
typedef uint16_t poly_coeff_t;
typedef uint32_t poly_dcoeff_t;
#else
typedef trint9_t poly_coeff_t;
typedef trint18_t poly_dcoeff_t;
#endif

// Type of polynomials mods (x^n+1, q)
typedef poly_coeff_t poly_t[MAM2_POLY_N];
#define MAM2_POLY_DEF(f) poly_t f

#if defined(MAM2_POLY_MRED_BINARY)
poly_coeff_t poly_coeff_mredd(poly_dcoeff_t m);
#endif

// f(x) := f(x) + 1; f(x)/3 is small
void poly_small3_add1(poly_t f);

// h(x) := 3 f(x); f(x) is small
void poly_small_mul3(poly_t f, poly_t h);

// h(x) := f(x) + g(x) mods (m(x), q)
void poly_add(poly_t f, poly_t g, poly_t h);

// h(x) := f(x) - g(x) mods (m(x), q)
void poly_sub(poly_t f, poly_t g, poly_t h);

/**
 * th := NTT(f) ⊛ NTT(g) mods (m(x), q)
 * h(x) ≡ f(x) * g(x) mods (m(x), q)
 */
void poly_conv(poly_t tf, poly_t tg, poly_t th);

// ∃? h(x) : 1 ≡ f(x) * h(x) mods (m(x), q), t = NTT(f)
bool poly_has_inv(poly_t t);

// h(x) := f⁻¹(x) mods (m(x), q), tf = NTT(f), th = NTT(h)
void poly_inv(poly_t tf, poly_t th);

/**
 * t(x) := NTT(f)
 * tⱼ = f(γ²ʲ⁺¹) ≡ Σᵢfᵢγ⁽²ʲ⁺¹⁾ⁱ
 */
void poly_ntt(poly_t f, poly_t t);

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
void poly_intt(poly_t t, poly_t f);

// tᵢ := fᵢ (mods 3)
void poly_round_to_trits(poly_t f, trits_t t);

// fᵢ := tᵢ
void poly_small_from_trits(poly_t f, trits_t t);

// fᵢ := tᵢ
bool poly_from_trits(poly_t f, trits_t t);

// tᵢ := fᵢ ???
void poly_to_trits(poly_t f, trits_t t);

/**
 * Convert integer into internal polynomial coefficient representation
 * The input integer must be within the range [-(Q-1)/2,...,(Q-1)/2]
 */
poly_coeff_t poly_coeff_from_trint9(trint9_t t);

/**
 * Convert internal polynomial coefficient representation into an integer
 * The output integer will be within the range [-(Q-1)/2,...,(Q-1)/2]
 */
trint9_t poly_coeff_to_trint9(poly_coeff_t c);

// a + b (mods q)
poly_coeff_t poly_coeff_add(poly_coeff_t a, poly_coeff_t b);

// a - b (mods q)
poly_coeff_t poly_coeff_sub(poly_coeff_t a, poly_coeff_t b);

// a * b (mods q)
poly_coeff_t poly_coeff_mul(poly_coeff_t a, poly_coeff_t b);

// a * b + c (mods q)
poly_coeff_t poly_coeff_mul_add(poly_coeff_t a, poly_coeff_t b, poly_coeff_t c);

poly_coeff_t poly_coeff_inv(poly_coeff_t a);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_V2_NTRU_POLY_H__
