
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

#include <stdio.h>

static char const trinary_alphabet[] =
    "NOPQRSTUVWXYZ9ABCDEFGHIJKLM";  // [-13..13]

tryte_t tryte_from_trits(trit_t t0, trit_t t1, trit_t t2) {
  return (tryte_t)t0 + (tryte_t)3 * (tryte_t)t1 + (tryte_t)9 * (tryte_t)t2;
}

MAM2_SAPI bool_t tryte_from_char(tryte_t *t, char c) {
  bool_t r = 1;

  if ('N' <= c && c <= 'Z')
    *t = (tryte_t)(c - 'N') - 13;
  else if ('A' <= c && c <= 'M')
    *t = (trint3_t)(c - 'A' + 1);
  else if ('9' == c)
    *t = (trint3_t)0;
  else
    // let everything else be zero?
    r = 0;

  return r;
}

MAM2_SAPI char tryte_to_char(tryte_t t) {
  MAM2_ASSERT((tryte_t)-13 <= t && t <= (tryte_t)13);

  return trinary_alphabet[t + 13];
}

/*! \brief Minimum of two values. */
MAM2_INLINE static size_t min(size_t k, size_t n) { return k < n ? k : n; }

MAM2_SAPI bool_t trits_is_empty(trits_t x) { return (x.n == x.d); }

MAM2_SAPI size_t trits_size(trits_t x) { return (x.n - x.d); }

MAM2_SAPI size_t trits_size_min(trits_t x, size_t s) {
  size_t n = min(trits_size(x), s);
  return n;
}

MAM2_SAPI trits_t trits_from_rep(size_t n, word_t *w) {
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

MAM2_SAPI trits_t trits_drop_min(trits_t x, size_t n) {
  x.d = min(x.n, x.d + n);
  return x;
}

MAM2_SAPI trits_t trits_pickup(trits_t x, size_t n) {
  MAM2_ASSERT(x.d >= n);
  x.d -= n;
  return x;
}

MAM2_SAPI trint1_t trits_get1(trits_t x) {
  MAM2_ASSERT(x.n > x.d);

#if defined(MAM2_TRINARY_WORD_REP_INT)
#if (MAM2_TRITS_PER_WORD == 1)
  MAM2_ASSERT_TRINT1(x.p[x.d]);
  return x.p[x.d];
#else
#error not implemented
#endif
#elif defined(MAM2_TRINARY_WORD_REP_PACKED)
  static trint1_t const ts[4] = {0, 1, -1, 0};
  word_t *w = &x.p[x.d / MAM2_TRITS_PER_WORD];
  size_t d = 2 * (x.d % MAM2_TRITS_PER_WORD);
  return ts[3 & (*w >> d)];
#elif defined(MAM2_TRINARY_WORD_REP_INTERLEAVED)
  static trint1_t const ts[2][2] = {{0, -1}, {1, 0}};
  word_t *w = &x.p[x.d / MAM2_TRITS_PER_WORD];
  size_t d = x.d % MAM2_TRITS_PER_WORD;
  return ts[1 & (w->lo >> d)][1 & (w->hi >> d)];
#endif
}

MAM2_SAPI void trits_put1(trits_t x, trint1_t t) {
  MAM2_ASSERT(x.n > x.d);
  MAM2_ASSERT_TRINT1(t);

#if defined(MAM2_TRINARY_WORD_REP_INT)
#if (MAM2_TRITS_PER_WORD == 1)
  x.p[x.d] = t;
#else
#error not implemented
#endif
#elif defined(MAM2_TRINARY_WORD_REP_PACKED)
  static word_t const ws[3] = {2, 0, 1};
  word_t *w = &x.p[x.d / MAM2_TRITS_PER_WORD];
  size_t d = 2 * (x.d % MAM2_TRITS_PER_WORD);
  *w = *w & ~((word_t)3 << d);
  *w = *w | (ws[t + 1] << d);
#elif defined(MAM2_TRINARY_WORD_REP_INTERLEAVED)
  static word_t const ws[3] = {{1, 0}, {0, 0}, {0, 1}};
  word_t *w = &x.p[x.d / MAM2_TRITS_PER_WORD];
  size_t d = x.d % MAM2_TRITS_PER_WORD;
  w->lo = w->lo & ~((rep_t)1 << d);
  w->lo = w->lo | (ws[t + 1].lo << d);
  w->hi = w->hi & ~((rep_t)1 << d);
  w->hi = w->hi | (ws[t + 1].hi << d);
#endif
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

MAM2_SAPI char trits_get_char(trits_t x) {
  trit_t ts[3] = {0, 0, 0};
  tryte_t i;

  trits_get(trits_take_min(x, 3), ts);
  i = 0 + 1 * ts[0] + 3 * ts[1] + 9 * ts[2];

  return tryte_to_char(i);
}

MAM2_SAPI bool_t trits_put_char(trits_t x, char c) {
  trit_t ts[3] = {0, 0, 0};
  tryte_t i = 0;
  size_t k;

  if (!tryte_from_char(&i, c)) return 0;

  ts[0] = MAM2_MODS(i, 3 * 3 * 3, 3);
  i = MAM2_DIVS(i, 3 * 3 * 3, 3);
  ts[1] = MAM2_MODS(i, 3 * 3, 3);
  i = MAM2_DIVS(i, 3 * 3, 3);
  ts[2] = MAM2_MODS(i, 3, 3);

  for (k = trits_size_min(x, 3); k < 3; ++k)
    if (0 != ts[k]) return 0;

  trits_put(trits_take_min(x, 3), ts);
  return 1;
}

MAM2_SAPI byte trits_get_byte(trits_t x) {
  trit_t ts[5] = {0, 0, 0, 0, 0};
  int i;

  trits_get(trits_take_min(x, 5), ts);
  i = 0 + 1 * ts[0] + 3 * ts[1] + 9 * ts[2] + 27 * ts[3] + 81 * ts[4];

  return (0 <= i) ? (byte)i : (byte)(243 + i);
}

MAM2_SAPI bool_t trits_put_byte(trits_t x, byte b) {
  trit_t ts[5] = {0, 0, 0, 0, 0};
  int i = (b <= 121) ? (int)b : ((int)b - 243);
  size_t k;

  if (242 < b) return 0;

  ts[0] = MAM2_MODS(i, 3 * 3 * 3 * 3 * 3, 3);
  i = MAM2_DIVS(i, 3 * 3 * 3 * 3 * 3, 3);
  ts[1] = MAM2_MODS(i, 3 * 3 * 3 * 3, 3);
  i = MAM2_DIVS(i, 3 * 3 * 3 * 3, 3);
  ts[2] = MAM2_MODS(i, 3 * 3 * 3, 3);
  i = MAM2_DIVS(i, 3 * 3 * 3, 3);
  ts[3] = MAM2_MODS(i, 3 * 3, 3);
  i = MAM2_DIVS(i, 3 * 3, 3);
  ts[4] = MAM2_MODS(i, 3, 3);

  for (k = trits_size_min(x, 5); k < 5; ++k)
    if (0 != ts[k]) return 0;

  trits_put(trits_take_min(x, 5), ts);
  return 1;
}

MAM2_SAPI void trits_get(trits_t x, trit_t *t) {
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) *t++ = trits_get1(x);
}

MAM2_SAPI void trits_put(trits_t x, trit_t *t) {
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) trits_put1(x, *t++);
}

MAM2_SAPI void trits_to_str(trits_t x, char *s) {
  for (; !trits_is_empty(x); x = trits_drop_min(x, 3)) *s++ = trits_get_char(x);
}

MAM2_SAPI bool_t trits_from_str(trits_t x, char *s) {
  bool_t r = 1;

  for (; !trits_is_empty(x); x = trits_drop_min(x, 3))
    r = trits_put_char(x, *s++);

  return r;
}

MAM2_SAPI void trits_to_bytes(trits_t x, byte *bs) {
  for (; !trits_is_empty(x); x = trits_drop_min(x, 5))
    *bs++ = trits_get_byte(x);
}

MAM2_SAPI bool_t trits_from_bytes(trits_t x, byte *bs) {
  bool_t r = 1;

  for (; r && !trits_is_empty(x); x = trits_drop_min(x, 5))
    r = trits_put_byte(x, *bs++);

  return r;
}

MAM2_SAPI void trits_set_zero(trits_t x) {
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) trits_put1(x, 0);
}

MAM2_SAPI void trits_copy(trits_t x, trits_t y) {
  MAM2_ASSERT(trits_size(x) == trits_size(y));
  MAM2_ASSERT(trits_is_same(x, y) || !trits_is_overlapped(x, y));

  for (; !trits_is_empty(x); x = trits_drop(x, 1), y = trits_drop(y, 1))
    trits_put1(y, trits_get1(x));
}

MAM2_SAPI size_t trits_copy_min(trits_t x, trits_t y) {
  size_t n = min(trits_size(x), trits_size(y));
  trits_copy(trits_take(x, n), trits_take(y, n));
  return n;
}

MAM2_SAPI void trits_pad10(trits_t y) {
  MAM2_ASSERT(!trits_is_empty(y));

  // 1
  trits_put1(y, 1);
  // 0*
  trits_set_zero(trits_drop(y, 1));
}

MAM2_SAPI void trits_copy_pad10(trits_t x, trits_t y) {
  size_t n = trits_size(x);
  MAM2_ASSERT(n < trits_size(y));

  trits_copy(x, trits_take(y, n));
  y = trits_drop(y, n);
  trits_pad10(y);
}

void trits_add(trits_t x, trits_t s, trits_t y) {
  trit_t tx, ts, ty;

  MAM2_ASSERT(trits_size(x) == trits_size(s));
  MAM2_ASSERT(trits_size(x) == trits_size(y));
  MAM2_ASSERT(trits_is_same(x, y) || !trits_is_overlapped(x, y));

  for (; !trits_is_empty(x);
       x = trits_drop(x, 1), s = trits_drop(s, 1), y = trits_drop(y, 1)) {
    tx = trits_get1(x);
    ts = trits_get1(s);
    ty = trit_add(tx, ts);
    trits_put1(y, ty);
  }
}

void trits_sub(trits_t y, trits_t s, trits_t x) {
  trit_t tx, ts, ty;

  MAM2_ASSERT(trits_size(x) == trits_size(s));
  MAM2_ASSERT(trits_size(x) == trits_size(y));
  MAM2_ASSERT(trits_is_same(x, y) || !trits_is_overlapped(x, y));

  for (; !trits_is_empty(x);
       x = trits_drop(x, 1), s = trits_drop(s, 1), y = trits_drop(y, 1)) {
    ty = trits_get1(y);
    ts = trits_get1(s);
    tx = trit_sub(ty, ts);
    trits_put1(x, tx);
  }
}

MAM2_SAPI void trits_copy_add(trits_t x, trits_t s, trits_t y) {
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

MAM2_SAPI void trits_copy_sub(trits_t y, trits_t s, trits_t x) {
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

MAM2_SAPI void trits_swap_add(trits_t x, trits_t s) {
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

MAM2_SAPI void trits_swap_sub(trits_t y, trits_t s) {
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

MAM2_INLINE MAM2_SAPI bool_t trits_is_same(trits_t x, trits_t y) {
  return (x.p == y.p) && (x.d == y.d);  // && (x.n == y.n)
}

MAM2_INLINE MAM2_SAPI bool_t trits_is_overlapped(trits_t x, trits_t y) {
  word_t *x_first = x.p + (x.d / MAM2_TRITS_PER_WORD);
  word_t *x_last =
      x.p + (x.n / MAM2_TRITS_PER_WORD) + ((x.n % MAM2_TRITS_PER_WORD) ? 1 : 0);
  word_t *y_first = y.p + (y.d / MAM2_TRITS_PER_WORD);
  word_t *y_last =
      y.p + (y.n / MAM2_TRITS_PER_WORD) + ((y.n % MAM2_TRITS_PER_WORD) ? 1 : 0);
  return (x_first < y_last) && (y_first < x_last);
}

MAM2_INLINE MAM2_SAPI trits_t trits_diff(trits_t begin, trits_t end) {
  MAM2_ASSERT(trits_is_overlapped(begin, end));
  MAM2_ASSERT(begin.p == end.p);
  MAM2_ASSERT(begin.n == end.n);
  MAM2_ASSERT(begin.d <= end.d);

  return trits_from_rep(end.d - begin.d, begin.p);
}

MAM2_SAPI trits_t trits_null() { return trits_from_rep(0, 0); }

MAM2_SAPI bool_t trits_is_null(trits_t x) { return (0 == x.p); }

MAM2_SAPI bool_t trits_inc(trits_t x) {
  trit_t t;
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) {
    t = trit_add(trits_get1(x), 1);
    trits_put1(x, t);
    if (0 != t) return 1;
  }
  return 0;
}

MAM2_SAPI trits_t trits_alloc(ialloc *a, size_t n) {
  word_t *p = mam2_words_alloc(a, MAM2_WORDS(n));
  return trits_from_rep(n, p);
}

MAM2_SAPI void trits_free(ialloc *a, trits_t x) { mam2_words_free(a, x.p); }

MAM2_SAPI void trits_print(trits_t x) {
  for (; !trits_is_empty(x); x = trits_drop_min(x, 3))
    printf("%c", trits_get_char(x));
}

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