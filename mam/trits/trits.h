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
#ifndef __MAM_TRITS_TRITS_H__
#define __MAM_TRITS_TRITS_H__

#include <string.h>

#include "common/errors.h"
#include "mam/defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Array of trits.
`p -> |...d...|......|`
*/
typedef struct trits_s {
  size_t n;  /*!< total number of trits pointed to by `p` */
  size_t d;  /*!< offset of the first trit; number of available trits is `n-d` */
  trit_t *p; /*!< pointer to the trits */
} trits_t;

#define MAM_TRITS_INIT(X, k) trits_from_rep(k, X##_p)

#define MAM_TRITS_DECL(X, k) trit_t X##_p[k]

#define MAM_TRITS_DEF(X, k) \
  MAM_TRITS_DECL(X, k);     \
  trits_t X

static inline trit_t *trits_begin(trits_t x) { return x.p + x.d; }
static inline trit_t *trits_end(trits_t x) { return x.p + x.d + x.n; }

/*! \brief Check `x.n` against zero. */
bool trits_is_empty(trits_t x);

/*! \brief Size of `x`. */
size_t trits_size(trits_t x);

/*! \brief Minimum of the size of `x` and `s`. */
size_t trits_size_min(trits_t x, size_t s);

/*! \brief Construct `n` trits from representation `w`. */
trits_t trits_from_rep(size_t const n, trit_t const *const t);

/*! \brief Take the first `n` trits from `x`. */
trits_t trits_take(trits_t x, size_t n);

/*! \brief Take at most `n` first trits from `x`. */
trits_t trits_take_min(trits_t x, size_t n);

/*! \brief Drop the first `n` trits from `x`. */
trits_t trits_drop(trits_t x, size_t n);

/*! \brief Drop at most `n` first trits from `x`. */
trits_t trits_drop_min(trits_t x, size_t n);

/*! \brief Pickup `n` trits previously dropped from `x`. */
trits_t trits_pickup(trits_t x, size_t n);

trits_t trits_pickup_all(trits_t x);

trits_t trits_dropped_all(trits_t x);

trits_t trits_advance(trits_t *b, size_t n);

/*! \brief Get the first trit. */
trint1_t trits_get1(trits_t x);

/*! \brief Put the first trit. */
void trits_put1(trits_t x, trint1_t t);

/*! \brief Get the first tryte. */
trint3_t trits_get3(trits_t x);

/*! \brief Put the first tryte. */
void trits_put3(trits_t x, trint3_t t);

trint6_t trits_get6(trits_t x);
void trits_put6(trits_t x, trint6_t t);
trint9_t trits_get9(trits_t x);
void trits_put9(trits_t x, trint9_t t);
trint18_t trits_get18(trits_t x);
void trits_put18(trits_t x, trint18_t t);

char trits_get_char(trits_t x);
bool trits_put_char(trits_t x, char c);

/*! \brief Check whether `x` and `y` point to the same memory location.
\note `trits_is_same(x, y)` implies `0 == trits_cmp_grlex(x, y)` but not vice
versa. */
static inline bool trits_is_same(trits_t x, trits_t y) { return (x.p == y.p) && (x.d == y.d); /* && (x.n == y.n) */ }

/*! \brief Check whether `x` and `y` point to overlapped memory location. */
static inline bool trits_is_overlapped(trits_t x, trits_t y) {
  trit_t *x_first = x.p + x.d;
  trit_t *x_last = x.p + x.n;
  trit_t *y_first = y.p + y.d;
  trit_t *y_last = y.p + y.n;
  return (x_first < y_last) && (y_first < x_last);
}

/*! \brief Get trits: `t`[i] := `x`[i]. */
static inline void trits_get(trits_t x, trit_t *t) { memcpy(t, x.p + x.d, trits_size(x)); }

/*! \brief Put trits: `x`[i] := `t`[i]. */
static inline void trits_put(trits_t x, trit_t *t) { memcpy(x.p + x.d, t, trits_size(x)); }

/*! \brief Convert trytes to string.
\note `trits_size(x)` must be multiple of 3.
Size of `s` must be equal `trits_size(x)/3`.
*/
void trits_to_str(trits_t x, char *s);

/*! \brief Convert trytes from string.
\note `trits_size(x)` must be multiple of 3.
Size of `s` must be equal `trits_size(x)/3`.
*/
bool trits_from_str(trits_t x, char const *s);

/* \brief Maximal value of size_t value = (27^13-1)/2. */
#define MAM_TRITS_SIZE_MAX 2026277576509488133ull
#define MAM_TRITS_MAX_SIZE_T_TRYTES ((5 * sizeof(size_t) + 2) / 3)
#define MAM_TRITS_MAX_SIZEOF_SIZE_T (3 * (MAM_TRITS_MAX_SIZE_T_TRYTES + 1))

size_t trits_sizeof_size_t(size_t n);
void trits_encode_size_t(size_t n, trits_t *b);
retcode_t trits_decode_size_t(size_t *const n, trits_t *const buffer);

/*! \brief Set zero trits: `x` := t^n. */
static inline void trits_set1(trits_t x, trit_t t) {
  MAM_ASSERT_TRINT1(t);
  memset(x.p + x.d, t, trits_size(x));
}

/*! \brief Set zero trits: `x` := 0^n. */
static inline void trits_set_zero(trits_t x) { trits_set1(x, 0); }

/*! \brief Copy trits: `y` := `x`. */
static inline void trits_copy(trits_t x, trits_t y) {
  MAM_ASSERT(trits_size(x) == trits_size(y));
  MAM_ASSERT(trits_is_same(x, y) || !trits_is_overlapped(x, y));

  memcpy(y.p + y.d, x.p + x.d, trits_size(x));
}

/*! \brief Copy minimal number of trits contained in `x` and `y`.
Return number of trits copied. */
size_t trits_copy_min(trits_t x, trits_t y);

/*! \brief Pad trits: `y := c0 || 0^{|y|-1}`. */
void trits_padc0(trit_t c0, trits_t y);

/*! \brief Pad non-empty trits: `y := c0 || 0^{|y|-1}` if `|y|>0`. */
void trits_padc(trit_t c0, trits_t y);

/*! \brief Copy and add trits: `y` := `x` + `s`, `s` := `x`. */
void trits_copy_add(trits_t x, trits_t s, trits_t y);

/*! \brief Copy and sub trits: `x` := `y` - `s`, `s` := `x`. */
void trits_copy_sub(trits_t y, trits_t s, trits_t x);

void trits_swap_add(trits_t x, trits_t s);
void trits_swap_sub(trits_t y, trits_t s);

void trit_swap_add(trit_t *x, trit_t *s);
void trit_swap_sub(trit_t *y, trit_t *s);

size_t trits_copy_add_min(trits_t x, trits_t s, trits_t y);
size_t trits_copy_sub_min(trits_t x, trits_t s, trits_t y);
size_t trits_swap_add_min(trits_t x, trits_t s);
size_t trits_swap_sub_min(trits_t x, trits_t s);

/*! \brief Compare trits: `x` <=> `y`. */
int trits_cmp_grlex(trits_t x, trits_t y);

/*! \brief Compare trits: `x` =? `y`. */
bool trits_cmp_eq(trits_t x, trits_t y);

bool trits_cmp_eq_str(trits_t x, char const *y);

/*! \brief Return `x` such that:
`trits_is_same(trits_drop(begin, trits_size(x)), end)` and
`trits_is_same(trits_take(begin, trits_size(x)), x)`.
*/
trits_t trits_diff(trits_t begin, trits_t end);

/*! \brief Null trits. */
trits_t trits_null();

/*! \brief Check `x.p` against null.
\note Usually trits_t can't be null. All basic layers including
`trits`, `sponge`, `prng`, `wots`, and `mss` rely on caller
to allocate memory for trits. But in certain cases where the size of memory
is difficult to trac memory can be allocated within a callee.
In such case trits should be passed by pointer: `trits_t *x`.
*/
bool trits_is_null(trits_t x);

/*! \brief Increment trits with carry. Return false if overflow would occur. */
bool trits_inc(trits_t x);

/*! \brief Alloc `n` trits. */
trits_t trits_alloc(size_t n);

/*! \brief Free trits `x`. */
void trits_free(trits_t x);

#ifdef __cplusplus
}
#endif

#endif  // __MAM_TRITS_TRITS_H__

/** @} */
