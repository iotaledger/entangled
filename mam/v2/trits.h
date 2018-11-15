
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
\file trits.h
\brief Basic trinary array operations.
*/

#ifndef __MAM_V2_TRITS_H__
#define __MAM_V2_TRITS_H__

#include "alloc.h"
#include "defs.h"

#include "common/trinary/flex_trit.h"
#include "common/trinary/trit_tryte.h"

/*! \brief Array of trits.
`p -> |...d...|......|`
*/
typedef struct _trits_t {
  size_t n; /*!< total number of trits pointed to by `p` */
  size_t d; /*!< offset of the first trit; number of available trits is `n-d` */
  trit_t *p; /*!< pointer to the words holding trits */
} trits_t;

/*! \brief Minimum of two values. */
MAM2_INLINE static size_t min(size_t k, size_t n) { return k < n ? k : n; }

#define MAM2_TRITS_INIT(X, k) trits_from_rep(k, X##_p)

#define MAM2_TRITS_DECL(X, k)  \
  trit_t X##_p[MAM2_WORDS(k)]; \
  memset(X##_p, 0, MAM2_WORDS(k));

#define MAM2_TRITS_DEF(X, k) \
  MAM2_TRITS_DECL(X, k);     \
  trits_t X = MAM2_TRITS_INIT(X, k);

/*! \brief Check `x.n` against zero. */
MAM2_INLINE MAM2_API bool_t trits_is_empty(trits_t x);

/*! \brief Size of `x`. */
MAM2_INLINE MAM2_API size_t trits_size(trits_t x);

/*! \brief Construct `n` trits from representation `w`. */
MAM2_INLINE MAM2_API trits_t trits_from_rep(size_t n, trit_t *w);

/*! \brief Take the first `n` trits from `x`. */
MAM2_API trits_t trits_take(trits_t x, size_t n);

/*! \brief Take at most `n` first trits from `x`. */
MAM2_API trits_t trits_take_min(trits_t x, size_t n);

/*! \brief Drop the first `n` trits from `x`. */
MAM2_API trits_t trits_drop(trits_t x, size_t n);

/*! \brief Get the first trit. */
MAM2_API int64_t trits_get1(trits_t x);

/*! \brief Put the first trit. */
MAM2_API void trits_put1(trits_t x, trint1_t t);

/*! \brief Get the first tryte. */
MAM2_API trint3_t trits_get3(trits_t x);

/*! \brief Put the first tryte. */
MAM2_API void trits_put3(trits_t x, trint3_t t);

MAM2_API trint6_t trits_get6(trits_t x);
MAM2_API void trits_put6(trits_t x, trint6_t t);
MAM2_API trint9_t trits_get9(trits_t x);
MAM2_API void trits_put9(trits_t x, trint9_t t);
MAM2_API trint18_t trits_get18(trits_t x);
MAM2_API void trits_put18(trits_t x, trint18_t t);

/*! \brief Convert trytes from string.
\note `trits_size(x)` must be multiple of 3.
Size of `s` must be equal `trits_size(x)/3`.
*/
MAM2_API bool_t trits_from_str(trits_t x, char *s);

/*! \brief Set zero trits: `x` := 0^n. */
MAM2_API void trits_set_zero(trits_t x);

/*! \brief Copy trits: `y` := `x`. */
MAM2_API void trits_copy(trits_t x, trits_t y);

/*! \brief Compare trits: `x` <=> `y`. */
MAM2_API int trits_cmp_grlex(trits_t x, trits_t y);

/*! \brief Compare trits: `x` =? `y`. */
MAM2_API bool_t trits_cmp_eq(trits_t x, trits_t y);

/*! \brief Return `x` such that:
`trits_is_same(trits_drop(begin, trits_size(x)), end)` and
`trits_is_same(trits_take(begin, trits_size(x)), x)`.
*/
MAM2_INLINE MAM2_API trits_t trits_diff(trits_t begin, trits_t end);

/*! \brief Null trits. */
MAM2_API trits_t trits_null();

/*! \brief Check `x.p` against null.
\note Usually trits_t can't be null. All basic layers including
`trits`, `sponge`, `prng`, `wots`, and `mss` rely on caller
to allocate memory for trits. But in certain cases where the size of memory
is difficult to trac memory can be allocated within a callee.
In such case trits should be passed by pointer: `trits_t *x`.
*/
MAM2_INLINE MAM2_API bool_t trits_is_null(trits_t x);

/*! \brief Alloc `n` trits. */
MAM2_API trits_t trits_alloc(size_t n);

/*! \brief Free trits `x`. */
MAM2_API void trits_free(trits_t x);

/*! \brief Return `x + s (mods 3)`. */
MAM2_API trit_t trit_add(trit_t x, trit_t s);

/*! \brief Return `y - s (mods 3)`. */
MAM2_API trit_t trit_sub(trit_t y, trit_t s);

/*! \brief Copy and pad trits: `y` := pad10(`x`). */
MAM2_API void trits_copy_pad10(trits_t x, trits_t y);

/*! \brief Pad trits: `y` := pad10(`y`). */
MAM2_API void trits_pad10(trits_t y);

/*! \brief Check whether `x` and `y` point to the same memory location.
\note `trits_is_same(x, y)` implies `0 == trits_cmp_grlex(x, y)` but not vice
versa. */
MAM2_INLINE MAM2_API bool_t trits_is_same(trits_t x, trits_t y);

/*! \brief Copy and add trits: `y` := `x` + `s`, `s` := `x`. */
MAM2_API void trits_copy_add(trits_t x, trits_t s, trits_t y);
/*! \brief Copy and sub trits: `x` := `y` - `s`, `s` := `x`. */
MAM2_API void trits_copy_sub(trits_t y, trits_t s, trits_t x);
MAM2_API void trits_swap_add(trits_t x, trits_t s);
MAM2_API void trits_swap_sub(trits_t y, trits_t s);

void flex_from_trits(trits_t t, flex_trit_t *flex_trits);

/*! \brief Print string rep of `x` into stdout if MAM2_DEBUG defined. */
#ifdef MAM2_DEBUG
#define trits_dbg_print(x) trits_print(x)
#else
#define trits_dbg_print(x)
#endif

#endif  // __MAM_V2_TRITS_H__
