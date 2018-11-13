
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
\file trits.c
\brief Basic trinary array operations.
*/
#include "mam/v2/trits.h"

#include <memory.h>
#include <stdio.h>

MAM2_SAPI bool_t trits_is_empty(trits_t x) { return (x.n == x.d); }

MAM2_SAPI size_t trits_size(trits_t x) { return (x.n - x.d); }

MAM2_SAPI trits_t trits_from_rep(size_t n, trit_t *w) {
  trits_t x = {n, 0, w};
  return x;
}

MAM2_SAPI trits_t trits_take(trits_t x, size_t n) {
  MAM2_ASSERT(x.n >= x.d + n);
  x.n = x.d + n;
  return x;
}

MAM2_SAPI trits_t trits_take_min(trits_t x, size_t n) {
  x.n = min(x.n, x.d + n);
  return x;
}

MAM2_SAPI trits_t trits_drop(trits_t x, size_t n) {
  MAM2_ASSERT(x.n >= x.d + n);
  x.d += n;
  return x;
}

MAM2_SAPI int64_t trits_get1(trits_t x) {
  MAM2_ASSERT(x.n > x.d);
  return x.p[x.d];
}

MAM2_SAPI void trits_put1(trits_t x, trint1_t t) {
  MAM2_ASSERT(x.n > x.d);
  MAM2_ASSERT_TRINT1(t);
  x.p[x.d] = t;
}

MAM2_SAPI trint3_t trits_get3(trits_t x) {
  MAM2_ASSERT(x.n >= x.d + 3);

  trint3_t t0 = (trint3_t)trits_get1(x);
  trint3_t t1 = (trint3_t)trits_get1(trits_drop(x, 1));
  trint3_t t2 = (trint3_t)trits_get1(trits_drop(x, 2));
  return t0 + (trint3_t)3 * t1 + (trint3_t)9 * t2;
}

MAM2_SAPI void trits_put3(trits_t x, trint3_t t) {
  MAM2_ASSERT(x.n >= x.d + 3);

  trint1_t t0 = MAM2_MODS(t, 3 * 3 * 3, 3);
  t = MAM2_DIVS(t, 3 * 3 * 3, 3);
  trint1_t t1 = MAM2_MODS(t, 3 * 3, 3);
  t = MAM2_DIVS(t, 3 * 3, 3);
  trint1_t t2 = (trint1_t)t;

  trits_put1(x, t0);
  trits_put1(trits_drop(x, 1), t1);
  trits_put1(trits_drop(x, 2), t2);
}

MAM2_SAPI trint6_t trits_get6(trits_t x) {
  MAM2_ASSERT(x.n >= x.d + 6);

  trint6_t t0 = (trint6_t)trits_get3(x);
  trint6_t t1 = (trint6_t)trits_get3(trits_drop(x, 3));
  return t0 + (trint6_t)27 * t1;
}

MAM2_SAPI void trits_put6(trits_t x, trint6_t t) {
  MAM2_ASSERT(x.n >= x.d + 6);

  trint3_t t0 = MAM2_MODS(t, 27 * 27, 27);
  t = MAM2_DIVS(t, 27 * 27, 27);
  trint3_t t1 = (trint3_t)t;

  trits_put3(x, t0);
  trits_put3(trits_drop(x, 3), t1);
}

MAM2_SAPI trint9_t trits_get9(trits_t x) {
  MAM2_ASSERT(x.n >= x.d + 9);

  trint9_t t0 = (trint9_t)trits_get3(x);
  trint9_t t1 = (trint9_t)trits_get3(trits_drop(x, 3));
  trint9_t t2 = (trint9_t)trits_get3(trits_drop(x, 6));
  return t0 + (trint9_t)27 * t1 + (trint9_t)729 * t2;
}

MAM2_SAPI void trits_put9(trits_t x, trint9_t t) {
  MAM2_ASSERT(x.n >= x.d + 9);

  trint3_t t0 = MAM2_MODS(t, 27 * 27 * 27, 27);
  t = MAM2_DIVS(t, 27 * 27 * 27, 27);
  trint3_t t1 = MAM2_MODS(t, 27 * 27, 27);
  t = MAM2_DIVS(t, 27 * 27, 27);
  trint3_t t2 = (trint3_t)t;

  trits_put3(x, t0);
  trits_put3(trits_drop(x, 3), t1);
  trits_put3(trits_drop(x, 6), t2);
}

MAM2_SAPI trint18_t trits_get18(trits_t x) {
  MAM2_ASSERT(x.n >= x.d + 18);

  trint18_t t0 = (trint18_t)trits_get9(x);
  trint18_t t1 = (trint18_t)trits_get9(trits_drop(x, 9));
  return t0 + (trint18_t)19683 * t1;
}

MAM2_SAPI void trits_put18(trits_t x, trint18_t t) {
  MAM2_ASSERT(x.n >= x.d + 18);

  trint9_t t0 = MAM2_MODS(t, 19683 * 19683, 19683);
  t = MAM2_DIVS(t, 19683 * 19683, 19683);
  trint9_t t1 = (trint9_t)t;

  trits_put9(x, t0);
  trits_put9(trits_drop(x, 9), t1);
}

MAM2_SAPI void trits_set_zero(trits_t x) {
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) trits_put1(x, 0);
}

MAM2_SAPI void trits_copy(trits_t x, trits_t y) {
  MAM2_ASSERT(trits_size(x) == trits_size(y));

  for (; !trits_is_empty(x); x = trits_drop(x, 1), y = trits_drop(y, 1))
    trits_put1(y, trits_get1(x));
}

MAM2_SAPI int trits_cmp_grlex(trits_t x, trits_t y) {
  int d = 0;

  if (trits_size(x) < trits_size(y)) return -3;
  if (trits_size(x) > trits_size(y)) return 3;

  for (; !trits_is_empty(x) && (0 == (d = trits_get1(x) - trits_get1(y)));
       x = trits_drop(x, 1), y = trits_drop(y, 1))
    ;

  return d;
}

MAM2_SAPI bool_t trits_cmp_eq(trits_t x, trits_t y) {
  return (0 == trits_cmp_grlex(x, y)) ? 1 : 0;
}

MAM2_INLINE MAM2_SAPI trits_t trits_diff(trits_t begin, trits_t end) {
  MAM2_ASSERT(begin.p == end.p);
  MAM2_ASSERT(begin.n == end.n);
  MAM2_ASSERT(begin.d <= end.d);

  return trits_from_rep(end.d - begin.d, begin.p);
}

MAM2_SAPI trits_t trits_null() { return trits_from_rep(0, 0); }

MAM2_SAPI bool_t trits_is_null(trits_t x) { return (0 == x.p); }

MAM2_SAPI trits_t trits_alloc(ialloc *a, size_t n) {
  trit_t *p = mam2_words_alloc(a, MAM2_WORDS(n));
  memset(p, 0, MAM2_WORDS(n));
  return trits_from_rep(n, p);
}

MAM2_SAPI void trits_free(ialloc *a, trits_t x) { mam2_words_free(a, x.p); }

#define MAM2_TRIT_NORM(t) (((t) == 2) ? -1 : ((t) == -2) ? 1 : (t))
trit_t trit_add(trit_t x, trit_t s) {
  trit_t y = x + s;
  y = MAM2_TRIT_NORM(y);
  return y;
}

trit_t trit_sub(trit_t y, trit_t s) {
  trit_t x = y - s;
  x = MAM2_TRIT_NORM(x);
  return x;
}
