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
\file defs.h
\brief MAM2 common definitions.
*/
#ifndef __MAM_V2_DEFS_H__
#define __MAM_V2_DEFS_H__

/*! \brief Enable debug output macros. */
#if 0
#define MAM2_DEBUG
#endif

/*! \brief MAM2 allocator type; nothing for now. */
typedef void ialloc;

#include <stddef.h>
#include <stdint.h>

#if defined(NDEBUG) /*&& defined(MAM2_DEBUG)*/
/* enable `assert` in RELEASE */
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

word_t should be thought of as ternary processor register.
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

#if defined(MAM2_TRINARY_WORD_REP_INT)
/*! \brief Single-trit word representation

`word_t` consists of one trit.

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
#error "Can't represent word_t for specified MAM2_TRITS_PER_WORD."
#endif
#endif

typedef MAM2_TRINARY_WORD_REP_INT_T rep_t;
typedef rep_t word_t;

#elif defined(MAM2_TRINARY_WORD_REP_INTERLEAVED)
/*! \brief Interleaved word representation

`word_t` is represented using two bit arrays (unsigned ints).
(lo,hi): (0,0)=(1,1)=0 (0,1)=1 (1,0)=-1

#define MAM2_TRINARY_WORD_REP_INTERLEAVED_T uint8_t
#define MAM2_TRITS_PER_WORD 8
or
#define MAM2_TRINARY_WORD_REP_INTERLEAVED_T uint32_t
#define MAM2_TRITS_PER_WORD 27
*/
#ifndef MAM2_TRINARY_WORD_REP_INTERLEAVED_T
#if (MAM2_TRITS_PER_WORD <= 8)
#define MAM2_TRINARY_WORD_REP_INTERLEAVED_T int8_t
#elif (MAM2_TRITS_PER_WORD <= 16)
#define MAM2_TRINARY_WORD_REP_INTERLEAVED_T int16_t
#elif (MAM2_TRITS_PER_WORD <= 32)
#define MAM2_TRINARY_WORD_REP_INTERLEAVED_T int32_t
#elif (MAM2_TRITS_PER_WORD <= 64)
#define MAM2_TRINARY_WORD_REP_INTERLEAVED_T int64_t
#else
#error "Can't represent word_t for specified MAM2_TRITS_PER_WORD."
#endif
#endif

typedef MAM2_TRINARY_WORD_REP_INTERLEAVED_T rep_t;
typedef struct {
  rep_t lo, hi;
} word_t;
#define MAM2_WORD_TRIT_IS_ZERO(w, t) \
  ((((w).lo ^ (w).hi) & ((word_t)1) << (t)) == 0)

#elif defined(MAM2_TRINARY_WORD_REP_PACKED)
/*#define MAM2_TRINARY_WORD_REP_PACKED_T uint8_t*/
/*#define MAM2_TRITS_PER_WORD 3*/
#ifndef MAM2_TRINARY_WORD_REP_PACKED_T
#if (MAM2_TRITS_PER_WORD <= 4)
#define MAM2_TRINARY_WORD_REP_PACKED_T int8_t
#elif (MAM2_TRITS_PER_WORD <= 8)
#define MAM2_TRINARY_WORD_REP_PACKED_T int16_t
#elif (MAM2_TRITS_PER_WORD <= 16)
#define MAM2_TRINARY_WORD_REP_PACKED_T int32_t
#elif (MAM2_TRITS_PER_WORD <= 32)
#define MAM2_TRINARY_WORD_REP_PACKED_T int64_t
#else
#error "Can't represent word_t for specified MAM2_TRITS_PER_WORD."
#endif
#endif

typedef MAM2_TRINARY_WORD_REP_PACKED_T rep_t;
typedef rep_t word_t;

#else
#error MAM2_TRINARY_WORD representation is not selected.	\
  Please, define one of MAM2_TRINARY_WORD_REP_INT, \
  MAM2_TRINARY_WORD_REP_INTERLEAVED, \
  or MAM2_TRINARY_WORD_REP_PACKED.
#endif

/*! \brief Minimum number of words needed to represent `t` trits */
#define MAM2_WORDS(t) (((t) + MAM2_TRITS_PER_WORD - 1) / MAM2_TRITS_PER_WORD)

/*! `M = 3^n`, `m = 3^k`, `n>k`. t \in [-(M-1)/2 .. (M-1)/2]. */
#define MAM2_MODS(t, M, m) ((((t) + ((M - 1) / 2)) % (m)) - ((m - 1) / 2))
#define MAM2_DIVS(t, M, m) ((((t) + ((M - 1) / 2)) / (m)) - ((M / m - 1) / 2))

/*! \brief Signed integer type capable of storing single trit
with values in range [-1,0,1]. */
typedef int8_t trint1_t;
#define MAM2_TRINT1_MAX ((trint1_t)1)
#define MAM2_TRINT1_MIN (-MAM2_TRINT1_MAX)
typedef trint1_t trit_t;

#define MAM2_ASSERT_TRINT1(t1)                                   \
  MAM2_ASSERT(((t1) == (trint1_t)-1) || ((t1) == (trint1_t)0) || \
              ((t1) == (trint1_t)1))

/*! \brief Return `x + s (mods 3)`. */
trit_t trit_add(trit_t x, trit_t s);

/*! \brief Return `y - s (mods 3)`. */
trit_t trit_sub(trit_t y, trit_t s);

/*! \brief Boolean type with zero as `false`(`no`) and non-zero as
 * `true`(`yes`). */
typedef trint1_t bool_t;

/*! \brief Signed integer type capable of storing 3 trits
with values in range [-13,..,-1,0,1,..,13]. */
typedef int8_t trint3_t;
#define MAM2_TRINT3_MAX ((trint3_t)13)
#define MAM2_TRINT3_MIN (-MAM2_TRINT3_MAX)
typedef trint3_t tryte_t;

tryte_t tryte_from_trits(trit_t t0, trit_t t1, trit_t t2);

char tryte_to_char(tryte_t t);

bool_t tryte_from_char(tryte_t *t, char c);

/*! \brief Signed integer type capable of storing 6 trits
with values in range [-(3^6-1)/2=-364,..,-1,0,1,..,364=(3^6-1)/2]. */
typedef int16_t trint6_t;
#define MAM2_TRINT6_MAX ((trint6_t)364)
#define MAM2_TRINT6_MIN (-MAM2_TRINT6_MAX)

/*! \brief Signed integer type capable of storing 9 trits
with values in range [-(3^9-1)/2=-9841,..,-1,0,1,..,9841=(3^9-1)/2]. */
typedef int16_t trint9_t;
#define MAM2_TRINT9_MAX ((trint9_t)9841)
#define MAM2_TRINT9_MIN (-MAM2_TRINT9_MAX)

/*! \brief Signed integer type capable of storing 18 trits
with values in range [-(3^18-1)/2,..,-1,0,1,..,(3^18-1)/2]. */
typedef int32_t trint18_t;
#define MAM2_TRINT18_MAX ((trint18_t)193710244)
#define MAM2_TRINT18_MIN (-MAM2_TRINT18_MAX)

/*! \note `trintX_t` types represent integer values whereas
`word_t` type represents fixed-size set of trits. */

/*! Unsigned 8-bit type. */
typedef uint8_t byte;

/*! \brief Assert expression. */
#define MAM2_ASSERT(expr) assert(expr)

#if 0
#define MAM2_POLY_MRED_BINARY
#endif

#endif  // __MAM_V2_DEFS_H__
