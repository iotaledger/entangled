
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 [ITSec Lab]

 *
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file defs.h
\brief MAM2 common definitions.
*/
#ifndef __MAM_V2_DEFS_H__
#define __MAM_V2_DEFS_H__

/*! \brief Enable debug output macros. */
//#define MAM2_DEBUG

#include <stddef.h>
#include <stdint.h>

#include "common/trinary/trits.h"

#if defined(NDEBUG)  //&& defined(MAM2_DEBUG)
// enable `assert` in RELEASE
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#endif
#include <assert.h>

#ifdef MAM2_DEBUG
#include <stdio.h>
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

/*! \brief

trit_t should be thought of as ternary processor register.
Although it can consist of several trytes
here it is considered as an atomic vector of trits.
In binary computer and in C language this type is tunable
and can be represented with uintX_t types of using binary-coded representation.
Client code should make no assumptions regarding it's representation
and should operate upon using macros presented in this module.

In order to optimize sequential processing at Sponge layer
a blocks of 486 trits should consist of whole number of words,
meaning 486 should be a multiple of TRITS_PER_WORD:
1,2,3,6,9,18,27,54,81.
Otherwise it would require inefficient shifts or unpacking.
*/

#if !defined(MAM2_TRITS_PER_WORD) || !(MAM2_TRITS_PER_WORD > 0)
#error MAM2_TRITS_PER_WORD is not defined or is not positive.
#endif

/*! \brief Single-trit word representation

`trit_t` consists of one trit.

#define MAM2_TRINARY_WORD_REP_INT_T int8_t
#define MAM2_TRITS_PER_WORD 1
or
#define MAM2_TRITS_PER_WORD 5
*/
#ifndef MAM2_TRINARY_WORD_REP_INT_T
#if (MAM2_TRITS_PER_WORD <= 5)
#define MAM2_TRINARY_WORD_REP_INT_T int8_t
#elif (MAM2_TRITS_PER_WORD <= 10)
#define MAM2_TRINARY_WORD_REP_INT_T int16_t
#elif (MAM2_TRITS_PER_WORD <= 20)
#define MAM2_TRINARY_WORD_REP_INT_T int32_t
#elif (MAM2_TRITS_PER_WORD <= 40)
#define MAM2_TRINARY_WORD_REP_INT_T int64_t
#else
#error "Can't represent trit_t for specified MAM2_TRITS_PER_WORD."
#endif
#endif

/*! \brief Minimum number of words needed to represent `t` trits */
#define MAM2_WORDS(t) (((t) + MAM2_TRITS_PER_WORD - 1) / MAM2_TRITS_PER_WORD)

/*! `M = 3^n`, `m = 3^k`, `n>k`. t \in [-(M-1)/2 .. (M-1)/2]. */
#define MAM2_MODS(t, M, m) ((((t) + ((M - 1) / 2)) % (m)) - ((m - 1) / 2))
#define MAM2_DIVS(t, M, m) ((((t) + ((M - 1) / 2)) / (m)) - ((M / m - 1) / 2))

/*! \brief Signed integer type capable of storing single trit
with values in range [-1,0,1]. */
typedef int8_t trint1_t;
typedef trint1_t trit_t;

#define MAM2_ASSERT_TRINT1(t1)                                   \
  MAM2_ASSERT(((t1) == (trint1_t)-1) || ((t1) == (trint1_t)0) || \
              ((t1) == (trint1_t)1))

/*! \brief Signed integer type capable of storing 3 trits
with values in range [-13,..,-1,0,1,..,13]. */
typedef int8_t trint3_t;
typedef trint3_t tryte_t;

/*! \brief Signed integer type capable of storing 6 trits
with values in range [-(3^6-1)/2=-364,..,-1,0,1,..,364=(3^6-1)/2]. */
typedef int16_t trint6_t;

/*! \brief Signed integer type capable of storing 9 trits
with values in range [-(3^9-1)/2=-9841,..,-1,0,1,..,9841=(3^9-1)/2]. */
typedef int16_t trint9_t;

/*! \brief Signed integer type capable of storing 18 trits
with values in range [-(3^18-1)/2,..,-1,0,1,..,(3^18-1)/2]. */
typedef int32_t trint18_t;

/*! \note `trintX_t` types represent integer values whereas
`trit_t` type represents fixed-size set of trits. */

/*! Unsigned 8-bit type. */
typedef uint8_t byte;

/*! \brief Assert expression. */
#define MAM2_ASSERT(expr) assert(expr)

#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
#define FLEX_TRIT_SIZE_18 18
#define FLEX_TRIT_SIZE_162 162
#define FLEX_TRIT_SIZE_234 234
#define FLEX_TRIT_SIZE_13122 13122
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
#define FLEX_TRIT_SIZE_18 6
#define FLEX_TRIT_SIZE_162 54
#define FLEX_TRIT_SIZE_234 78
#define FLEX_TRIT_SIZE_13122 4374
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
#define FLEX_TRIT_SIZE_18 5
#define FLEX_TRIT_SIZE_162 41
#define FLEX_TRIT_SIZE_234 59
#define FLEX_TRIT_SIZE_13122 3281
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
#define FLEX_TRIT_SIZE_18 4
#define FLEX_TRIT_SIZE_162 33
#define FLEX_TRIT_SIZE_234 47
#define FLEX_TRIT_SIZE_13122 2625
#endif

#endif  // __MAM_V2_DEFS_H__
