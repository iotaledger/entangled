
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

bool_t trits_is_empty(trits_t x) { return (x.n == x.d); }

size_t trits_size(trits_t x) { return (x.n - x.d); }

trits_t trits_from_rep(size_t n, trit_t *w) {
  trits_t x = {n, 0, w};
  return x;
}

trits_t trits_take(trits_t x, size_t n) {
  MAM2_ASSERT(x.n >= x.d + n);
  x.n = x.d + n;
  return x;
}

trits_t trits_take_min(trits_t x, size_t n) {
  x.n = min(x.n, x.d + n);
  return x;
}

trits_t trits_drop(trits_t x, size_t n) {
  MAM2_ASSERT(x.n >= x.d + n);
  x.d += n;
  return x;
}

int64_t trits_get1(trits_t x) {
  MAM2_ASSERT(x.n > x.d);
  return x.p[x.d];
}

void trits_put1(trits_t x, trint1_t t) {
  MAM2_ASSERT(x.n > x.d);
  MAM2_ASSERT_TRINT1(t);
  x.p[x.d] = t;
}

trint3_t trits_get3(trits_t x) {
  MAM2_ASSERT(x.n >= x.d + 3);

  trint3_t t0 = (trint3_t)trits_get1(x);
  trint3_t t1 = (trint3_t)trits_get1(trits_drop(x, 1));
  trint3_t t2 = (trint3_t)trits_get1(trits_drop(x, 2));
  return t0 + (trint3_t)3 * t1 + (trint3_t)9 * t2;
}

void trits_put3(trits_t x, trint3_t t) {
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

trint9_t trits_get9(trits_t x) {
  MAM2_ASSERT(x.n >= x.d + 9);

  trint9_t t0 = (trint9_t)trits_get3(x);
  trint9_t t1 = (trint9_t)trits_get3(trits_drop(x, 3));
  trint9_t t2 = (trint9_t)trits_get3(trits_drop(x, 6));
  return t0 + (trint9_t)27 * t1 + (trint9_t)729 * t2;
}

void trits_put9(trits_t x, trint9_t t) {
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

trint18_t trits_get18(trits_t x) {
  MAM2_ASSERT(x.n >= x.d + 18);

  trint18_t t0 = (trint18_t)trits_get9(x);
  trint18_t t1 = (trint18_t)trits_get9(trits_drop(x, 9));
  return t0 + (trint18_t)19683 * t1;
}

void trits_set_zero(trits_t x) {
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) trits_put1(x, 0);
}

void trits_copy(trits_t x, trits_t y) {
  MAM2_ASSERT(trits_size(x) == trits_size(y));

  while (!trits_is_empty(x)) {
    trits_put1(y, trits_get1(x));
    x = trits_drop(x, 1);
    y = trits_drop(y, 1);
  }
}

int trits_cmp_grlex(trits_t x, trits_t y) {
  int d = 0;

  if (trits_size(x) < trits_size(y)) return -3;
  if (trits_size(x) > trits_size(y)) return 3;

  for (; !trits_is_empty(x) && (0 == (d = trits_get1(x) - trits_get1(y)));
       x = trits_drop(x, 1), y = trits_drop(y, 1))
    ;

  return d;
}

bool_t trits_cmp_eq(trits_t x, trits_t y) {
  return (0 == trits_cmp_grlex(x, y)) ? 1 : 0;
}

trits_t trits_diff(trits_t begin, trits_t end) {
  MAM2_ASSERT(begin.p == end.p);
  MAM2_ASSERT(begin.n == end.n);
  MAM2_ASSERT(begin.d <= end.d);

  return trits_from_rep(end.d - begin.d, begin.p);
}

trits_t trits_null() { return trits_from_rep(0, 0); }

bool_t trits_is_null(trits_t x) { return (0 == x.p); }

trits_t trits_alloc(size_t n) {
  trit_t *p = (trit_t *)malloc(sizeof(trit_t) * MAM2_WORDS(n));
  memset(p, 0, MAM2_WORDS(n));
  return trits_from_rep(n, p);
}

void trits_free(trits_t x) { free(x.p); }

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

void trits_copy_pad10(trits_t x, trits_t y) {
  size_t n = trits_size(x);
  MAM2_ASSERT(n < trits_size(y));
  trits_copy(x, trits_take(y, n));
  y = trits_drop(y, n);
  trits_pad10(y);
}

void trits_pad10(trits_t y) {
  MAM2_ASSERT(!trits_is_empty(y));
  // 1
  trits_put1(y, 1);
  // 0*
  trits_set_zero(trits_drop(y, 1));
}

bool_t trits_is_same(trits_t x, trits_t y) {
  return (x.p == y.p) && (x.d == y.d);  // && (x.n == y.n)
}

void trits_copy_add(trits_t x, trits_t s, trits_t y) {
  trit_t tx, ts, ty;
  MAM2_ASSERT(trits_size(x) == trits_size(s));
  MAM2_ASSERT(trits_size(x) == trits_size(y));
  for (; !trits_is_empty(x);
       x = trits_drop(x, 1), s = trits_drop(s, 1), y = trits_drop(y, 1)) {
    tx = trits_get1(x);
    ts = trits_get1(s);
    ty = trit_add(tx, ts);
    trits_put1(s, tx);
    trits_put1(y, ty);
  }
}
void trits_copy_sub(trits_t y, trits_t s, trits_t x) {
  trit_t tx, ts, ty;
  MAM2_ASSERT(trits_size(x) == trits_size(s));
  MAM2_ASSERT(trits_size(x) == trits_size(y));
  for (; !trits_is_empty(x);
       x = trits_drop(x, 1), s = trits_drop(s, 1), y = trits_drop(y, 1)) {
    ty = trits_get1(y);
    ts = trits_get1(s);
    tx = trit_sub(ty, ts);
    trits_put1(s, tx);
    trits_put1(x, tx);
  }
}

void trit_swap_add(trit_t *x, trit_t *s) {
  trit_t y = *x + *s;
  y = MAM2_TRIT_NORM(y);
  *s = *x;
  *x = y;
}
void trit_swap_sub(trit_t *y, trit_t *s) {
  trit_t x = *y - *s;
  x = MAM2_TRIT_NORM(x);
  *s = x;
  *y = x;
}

void trits_swap_add(trits_t x, trits_t s) {
  trit_t tx, ts;
  MAM2_ASSERT(trits_size(x) == trits_size(s));
  for (; !trits_is_empty(x); x = trits_drop(x, 1), s = trits_drop(s, 1)) {
    tx = trits_get1(x);
    ts = trits_get1(s);
    trit_swap_add(&tx, &ts);
    trits_put1(x, tx);
    trits_put1(s, ts);
  }
}
void trits_swap_sub(trits_t y, trits_t s) {
  trit_t ty, ts;
  MAM2_ASSERT(trits_size(y) == trits_size(s));
  for (; !trits_is_empty(y); y = trits_drop(y, 1), s = trits_drop(s, 1)) {
    ty = trits_get1(y);
    ts = trits_get1(s);
    trit_swap_sub(&ty, &ts);
    trits_put1(y, ty);
    trits_put1(s, ts);
  }
}

void flex_trit_t_from_trits_t(trits_t t, flex_trit_t *flex_trits) {
  size_t n = t.n - t.d;
  flex_trits_from_trits(flex_trits, n, &t.p[t.d], n, n);
}
