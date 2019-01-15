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
\file poly.h
\brief MAM2 NTRU polynomials arithmetic.
*/
#ifndef __MAM_V2_NTRU_POLY_H__
#define __MAM_V2_NTRU_POLY_H__

#include "mam/v2/defs.h"
#include "mam/v2/trits/trits.h"

/*! \brief Use Montgomery reduction. */
/*#define MAM2_POLY_MRED_BINARY*/

/*! \brief Prime modulus q = 3*2^12+1.
\note Some algorithms are specifically crafted for this value. */
#define MAM2_POLY_Q 12289

/*! \brief Degree `n` of polynomial `m(x) = (x^n+1)`. */
/*#define MAM2_POLY_MRED_BINARY*/
#define MAM2_POLY_N_LOG 10
#define MAM2_POLY_N (1 << MAM2_POLY_N_LOG)

/*!
\brief γ²ⁿ ≡ γ γ⁻¹ ≡ 1 (mod q).
\brief ωⁿ  ≡ ω ω⁻¹ ≡ 1 (mod q). ω ≡ γ² (mod q).
\brief n n⁻¹ ≡ 1 (mod q).
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

/*! R = 2¹⁶. */
#define MAM2_POLY_MRED_R_LOG 16
/*! R⁻¹ = 9 2⁸ ≡ 2304 (mod q). */
#define MAM2_POLY_MRED_RI 2304
/*! q⁻¹ q - R R⁻¹ = 1 */
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

/*! \brief Type of polynomial coefficeints mods (q).
\note Depending on configuration coefficients may be
represented in Montgomery reduced form.
*/
#if defined(MAM2_POLY_MRED_BINARY)
typedef uint16_t poly_coeff_t;
typedef uint32_t poly_dcoeff_t;
#else
typedef trint9_t poly_coeff_t;
typedef trint18_t poly_dcoeff_t;
#endif

/*! \brief Type of polynomials mods (x^n+1, q). */
typedef poly_coeff_t poly_t[MAM2_POLY_N];
#define MAM2_POLY_DEF(f) poly_t f

/*! \brief h(x) := f(x) (mods 3) */
void poly_round_small(poly_t f, poly_t h);

/*! \brief f(x) := f(x) + 1; f(x)/3 is small */
void poly_small3_add1(poly_t f);

/*! \brief h(x) := 3 f(x); f(x) is small */
void poly_small_mul3(poly_t f, poly_t h);

/*! \brief h(x) := f(x) + g(x) mods (m(x), q) */
void poly_add(poly_t f, poly_t g, poly_t h);

/*! \brief h(x) := f(x) - g(x) mods (m(x), q) */
void poly_sub(poly_t f, poly_t g, poly_t h);

/*!
\brief th := NTT(f) ⊛ NTT(g) mods (m(x), q)
\note h(x) ≡ f(x) * g(x) mods (m(x), q)
*/
void poly_conv(poly_t tf, poly_t tg, poly_t th);

/*! \brief ∃? h(x) : 1 ≡ f(x) * h(x) mods (m(x), q), t = NTT(f) */
bool_t poly_has_inv(poly_t t);

/*! \brief h(x) := f⁻¹(x) mods (m(x), q), tf = NTT(f), th = NTT(h) */
void poly_inv(poly_t tf, poly_t th);

/*! \brief t(x) := NTT(f) */
void poly_ntt(poly_t f, poly_t t);

/*! \brief f(x) := NTT⁻¹(t) */
void poly_intt(poly_t t, poly_t f);

/*! \brief tᵢ := fᵢ (mods 3) */
void poly_round_to_trits(poly_t f, trits_t t);

/*! \brief fᵢ := tᵢ */
void poly_small_from_trits(poly_t f, trits_t t);

/*! \brief fᵢ := tᵢ */
bool_t poly_from_trits(poly_t f, trits_t t);

/*! \brief tᵢ := fᵢ ??? */
void poly_to_trits(poly_t f, trits_t t);

void poly_print(char const *s, poly_t f);

#endif  // __MAM_V2_NTRU_POLY_H__
