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
#ifndef __MAM_DEFS_H__
#define __MAM_DEFS_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined __EMSCRIPTEN__
#include <emscripten.h>
#define MAM_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define MAM_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

/*! `M = 3^n`, `m = 3^k`, `n>k`. t \in [-(M-1)/2 .. (M-1)/2]. */
#define MAM_MODS(t, M, m) ((((t) + ((M - 1) / 2)) % (m)) - ((m - 1) / 2))
#define MAM_DIVS(t, M, m) ((((t) + ((M - 1) / 2)) / (m)) - ((M / m - 1) / 2))

/*! \brief Signed integer type capable of storing single trit
with values in range [-1,0,1]. */
typedef int8_t trint1_t;
#define MAM_TRINT1_MAX ((trint1_t)1)
#define MAM_TRINT1_MIN (-MAM_TRINT1_MAX)
typedef trint1_t trit_t;

#define MAM_ASSERT_TRINT1(t1) MAM_ASSERT(((t1) == (trint1_t)-1) || ((t1) == (trint1_t)0) || ((t1) == (trint1_t)1))

/*! \brief Return `x + s (mods 3)`. */
trit_t trit_add(trit_t x, trit_t s);

/*! \brief Return `y - s (mods 3)`. */
trit_t trit_sub(trit_t y, trit_t s);

/*! \brief Signed integer type capable of storing 3 trits
with values in range [-13,..,-1,0,1,..,13]. */
typedef int8_t trint3_t;
#define MAM_TRINT3_MAX ((trint3_t)13)
#define MAM_TRINT3_MIN (-MAM_TRINT3_MAX)
typedef trint3_t tryte_t;

tryte_t tryte_from_trits(trit_t t0, trit_t t1, trit_t t2);

char tryte_to_char(tryte_t t);

bool tryte_from_char(tryte_t *t, char c);

/*! \brief Signed integer type capable of storing 6 trits
with values in range [-(3^6-1)/2=-364,..,-1,0,1,..,364=(3^6-1)/2]. */
typedef int16_t trint6_t;
#define MAM_TRINT6_MAX ((trint6_t)364)
#define MAM_TRINT6_MIN (-MAM_TRINT6_MAX)

/*! \brief Signed integer type capable of storing 9 trits
with values in range [-(3^9-1)/2=-9841,..,-1,0,1,..,9841=(3^9-1)/2]. */
typedef int16_t trint9_t;
#define MAM_TRINT9_MAX ((trint9_t)9841)
#define MAM_TRINT9_MIN (-MAM_TRINT9_MAX)

/*! \brief Signed integer type capable of storing 18 trits
with values in range [-(3^18-1)/2,..,-1,0,1,..,(3^18-1)/2]. */
typedef int32_t trint18_t;
#define MAM_TRINT18_MAX ((trint18_t)193710244)
#define MAM_TRINT18_MIN (-MAM_TRINT18_MAX)

/*! \note `trintX_t` types represent integer values whereas
`word_t` type represents fixed-size set of trits. */

/*! Unsigned 8-bit type. */
typedef uint8_t byte;

/*! \brief Assert expression. */
#define MAM_ASSERT(expr) assert(expr)

#ifdef __cplusplus
}
#endif

#endif  // __MAM_DEFS_H__

/** @} */
