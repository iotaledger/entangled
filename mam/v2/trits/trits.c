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
\file trits.c
\brief Basic trinary array operations.
*/

#include <memory.h>
#include <stdio.h>

#include "mam/v2/trits/trits.h"

static char const trinary_alphabet[] =
    "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"; /* [-13..13] */

tryte_t tryte_from_trits(trit_t t0, trit_t t1, trit_t t2) {
  return (tryte_t)t0 + (tryte_t)3 * (tryte_t)t1 + (tryte_t)9 * (tryte_t)t2;
}

bool_t tryte_from_char(tryte_t *t, char c) {
  bool_t r = 1;

  if ('N' <= c && c <= 'Z')
    *t = (tryte_t)(c - 'N') - 13;
  else if ('A' <= c && c <= 'M')
    *t = (trint3_t)(c - 'A' + 1);
  else if ('9' == c)
    *t = (trint3_t)0;
  else
    /* let everything else be zero? */
    r = 0;

  return r;
}

char tryte_to_char(tryte_t t) {
  MAM2_ASSERT((tryte_t)-13 <= t && t <= (tryte_t)13);

  return trinary_alphabet[t + 13];
}

/*! \brief Minimum of two values. */
static size_t min(size_t k, size_t n) { return k < n ? k : n; }

bool_t trits_is_empty(trits_t x) { return (x.n == x.d); }

size_t trits_size(trits_t x) { return (x.n - x.d); }

size_t trits_size_min(trits_t x, size_t s) {
  size_t n;
  n = min(trits_size(x), s);
  return n;
}

trits_t trits_from_rep(size_t n, word_t *w) {
  trits_t x;
  x.n = n;
  x.d = 0;
  x.p = w;
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

trits_t trits_drop_min(trits_t x, size_t n) {
  x.d = min(x.n, x.d + n);
  return x;
}

trits_t trits_pickup(trits_t x, size_t n) {
  MAM2_ASSERT(x.d >= n);
  x.d -= n;
  return x;
}

trits_t trits_pickup_all(trits_t x) {
  x.d = 0;
  return x;
}

trits_t trits_advance(trits_t *b, size_t n) {
  trits_t t;
  MAM2_ASSERT(b);
  t = trits_take(*b, n);
  *b = trits_drop(*b, n);
  return t;
}

trint1_t trits_get1(trits_t x) {
#if defined(MAM2_TRINARY_WORD_REP_INT)
#if (MAM2_TRITS_PER_WORD == 1)
  MAM2_ASSERT(!trits_is_empty(x));
  MAM2_ASSERT_TRINT1(x.p[x.d]);

  return x.p[x.d];
#else
#error not implemented
#endif
#elif defined(MAM2_TRINARY_WORD_REP_PACKED)
  static trint1_t const ts[4] = {0, 1, -1, 0};

  word_t *w;
  size_t d;
  MAM2_ASSERT(!trits_is_empty(x));

  w = &x.p[x.d / MAM2_TRITS_PER_WORD];
  d = 2 * (x.d % MAM2_TRITS_PER_WORD);
  return ts[3 & (*w >> d)];
#elif defined(MAM2_TRINARY_WORD_REP_INTERLEAVED)
  static trint1_t const ts[2][2] = {{0, -1}, {1, 0}};

  word_t *w;
  size_t d;
  MAM2_ASSERT(!trits_is_empty(x));

  w = &x.p[x.d / MAM2_TRITS_PER_WORD];
  d = x.d % MAM2_TRITS_PER_WORD;
  return ts[1 & (w->lo >> d)][1 & (w->hi >> d)];
#endif
}

void trits_put1(trits_t x, trint1_t t) {
#if defined(MAM2_TRINARY_WORD_REP_INT)
#if (MAM2_TRITS_PER_WORD == 1)
  MAM2_ASSERT(!trits_is_empty(x));
  MAM2_ASSERT_TRINT1(t);

  x.p[x.d] = t;
#else
#error not implemented
#endif
#elif defined(MAM2_TRINARY_WORD_REP_PACKED)
  static word_t const ws[3] = {2, 0, 1};

  word_t *w;
  size_t d;
  MAM2_ASSERT(!trits_is_empty(x));
  MAM2_ASSERT_TRINT1(t);

  w = &x.p[x.d / MAM2_TRITS_PER_WORD];
  d = 2 * (x.d % MAM2_TRITS_PER_WORD);
  *w = *w & ~((word_t)3 << d);
  *w = *w | (ws[t + 1] << d);
#elif defined(MAM2_TRINARY_WORD_REP_INTERLEAVED)
  static word_t const ws[3] = {{0, 1}, {0, 0}, {1, 0}};

  word_t *w;
  size_t d;
  MAM2_ASSERT(!trits_is_empty(x));
  MAM2_ASSERT_TRINT1(t);

  w = &x.p[x.d / MAM2_TRITS_PER_WORD];
  d = x.d % MAM2_TRITS_PER_WORD;
  w->lo = w->lo & ~((rep_t)1 << d);
  w->lo = w->lo | (ws[t + 1].lo << d);
  w->hi = w->hi & ~((rep_t)1 << d);
  w->hi = w->hi | (ws[t + 1].hi << d);
#endif
}

trint3_t trits_get3(trits_t x) {
  trint3_t t0, t1, t2;
  MAM2_ASSERT(trits_size(x) >= 3);

  t0 = (trint3_t)trits_get1(x);
  t1 = (trint3_t)trits_get1(trits_drop(x, 1));
  t2 = (trint3_t)trits_get1(trits_drop(x, 2));
  return t0 + (trint3_t)3 * t1 + (trint3_t)9 * t2;
}

void trits_put3(trits_t x, trint3_t t) {
  trint1_t t0, t1, t2;
  MAM2_ASSERT(trits_size(x) >= 3);

  t0 = MAM2_MODS(t, 3 * 3 * 3, 3);
  t = MAM2_DIVS(t, 3 * 3 * 3, 3);
  t1 = MAM2_MODS(t, 3 * 3, 3);
  t = MAM2_DIVS(t, 3 * 3, 3);
  t2 = (trint1_t)t;

  trits_put1(x, t0);
  trits_put1(trits_drop(x, 1), t1);
  trits_put1(trits_drop(x, 2), t2);
}

trint6_t trits_get6(trits_t x) {
  trint6_t t0, t1;
  MAM2_ASSERT(trits_size(x) >= 6);

  t0 = (trint6_t)trits_get3(x);
  t1 = (trint6_t)trits_get3(trits_drop(x, 3));
  return t0 + (trint6_t)27 * t1;
}

void trits_put6(trits_t x, trint6_t t) {
  trint3_t t0, t1;
  MAM2_ASSERT(trits_size(x) >= 6);

  t0 = MAM2_MODS(t, 27 * 27, 27);
  t = MAM2_DIVS(t, 27 * 27, 27);
  t1 = (trint3_t)t;

  trits_put3(x, t0);
  trits_put3(trits_drop(x, 3), t1);
}

trint9_t trits_get9(trits_t x) {
  trint9_t t0, t1, t2;
  MAM2_ASSERT(trits_size(x) >= 9);

  t0 = (trint9_t)trits_get3(x);
  t1 = (trint9_t)trits_get3(trits_drop(x, 3));
  t2 = (trint9_t)trits_get3(trits_drop(x, 6));
  return t0 + (trint9_t)27 * t1 + (trint9_t)729 * t2;
}

void trits_put9(trits_t x, trint9_t t) {
  trint3_t t0, t1, t2;
  MAM2_ASSERT(trits_size(x) >= 9);

  t0 = MAM2_MODS(t, 27 * 27 * 27, 27);
  t = MAM2_DIVS(t, 27 * 27 * 27, 27);
  t1 = MAM2_MODS(t, 27 * 27, 27);
  t = MAM2_DIVS(t, 27 * 27, 27);
  t2 = (trint3_t)t;

  trits_put3(x, t0);
  trits_put3(trits_drop(x, 3), t1);
  trits_put3(trits_drop(x, 6), t2);
}

trint18_t trits_get18(trits_t x) {
  trint18_t t0, t1;
  MAM2_ASSERT(trits_size(x) >= 18);

  t0 = (trint18_t)trits_get9(x);
  t1 = (trint18_t)trits_get9(trits_drop(x, 9));
  return t0 + (trint18_t)19683 * t1;
}

void trits_put18(trits_t x, trint18_t t) {
  trint9_t t0, t1;
  MAM2_ASSERT(trits_size(x) >= 18);

  t0 = MAM2_MODS(t, 19683 * 19683, 19683);
  t = MAM2_DIVS(t, 19683 * 19683, 19683);
  t1 = (trint9_t)t;

  trits_put9(x, t0);
  trits_put9(trits_drop(x, 9), t1);
}

char trits_get_char(trits_t x) {
  trit_t ts[3] = {0, 0, 0};
  tryte_t i;

  trits_get(trits_take_min(x, 3), ts);
  i = 0 + 1 * ts[0] + 3 * ts[1] + 9 * ts[2];

  return tryte_to_char(i);
}

bool_t trits_put_char(trits_t x, char c) {
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

byte trits_get_byte(trits_t x) {
  trit_t ts[5] = {0, 0, 0, 0, 0};
  int i;

  trits_get(trits_take_min(x, 5), ts);
  i = 0 + 1 * ts[0] + 3 * ts[1] + 9 * ts[2] + 27 * ts[3] + 81 * ts[4];

  return (0 <= i) ? (byte)i : (byte)(243 + i);
}

bool_t trits_put_byte(trits_t x, byte b) {
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

void trits_get(trits_t x, trit_t *t) {
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) *t++ = trits_get1(x);
}

void trits_put(trits_t x, trit_t *t) {
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) trits_put1(x, *t++);
}

void trits_to_str(trits_t x, char *s) {
  for (; !trits_is_empty(x); x = trits_drop_min(x, 3)) *s++ = trits_get_char(x);
}

bool_t trits_from_str(trits_t x, char const *s) {
  bool_t r = 1;

  for (; r && !trits_is_empty(x); x = trits_drop_min(x, 3))
    r = trits_put_char(x, *s++);

  return r;
}

void trits_to_bytes(trits_t x, byte *bs) {
  for (; !trits_is_empty(x); x = trits_drop_min(x, 5))
    *bs++ = trits_get_byte(x);
}

bool_t trits_from_bytes(trits_t x, byte const *bs) {
  bool_t r = 1;

  for (; r && !trits_is_empty(x); x = trits_drop_min(x, 5))
    r = trits_put_byte(x, *bs++);

  return r;
}

void trits_set1(trits_t x, trit_t t) {
  MAM2_ASSERT_TRINT1(t);
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) trits_put1(x, t);
}

void trits_set_zero(trits_t x) { trits_set1(x, 0); }

void trits_copy(trits_t x, trits_t y) {
  MAM2_ASSERT(trits_size(x) == trits_size(y));
  MAM2_ASSERT(trits_is_same(x, y) || !trits_is_overlapped(x, y));

  for (; !trits_is_empty(x); x = trits_drop(x, 1), y = trits_drop(y, 1))
    trits_put1(y, trits_get1(x));
}

size_t trits_copy_min(trits_t x, trits_t y) {
  size_t n;
  n = min(trits_size(x), trits_size(y));
  trits_copy(trits_take(x, n), trits_take(y, n));
  return n;
}

void trits_padc0(trit_t c0, trits_t y) {
  MAM2_ASSERT(!trits_is_empty(y));

  /* c0 */
  trits_put1(y, c0);
  /* 0* */
  trits_set_zero(trits_drop(y, 1));
}

void trits_copy_padc0(trit_t c0, trits_t x, trits_t y) {
  size_t n = trits_size(x);
  MAM2_ASSERT(n < trits_size(y));

  trits_copy(x, trits_take(y, n));
  y = trits_drop(y, n);
  trits_padc0(c0, y);
}

void trits_padc(trit_t c0, trits_t y) {
  if (!trits_is_empty(y)) trits_padc0(c0, y);
}

void trits_copy_padc(trit_t c0, trits_t x, trits_t y) {
  size_t n = trits_size(x);
  MAM2_ASSERT(n <= trits_size(y));

  trits_copy(x, trits_take(y, n));
  y = trits_drop(y, n);
  trits_padc(c0, y);
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
static void trit_swap_add(trit_t *x, trit_t *s) {
  trit_t y = *x + *s;
  y = MAM2_TRIT_NORM(y);
  *s = *x;
  *x = y;
}
static void trit_swap_sub(trit_t *y, trit_t *s) {
  trit_t x = *y - *s;
  x = MAM2_TRIT_NORM(x);
  *s = x;
  *y = x;
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

size_t trits_copy_add_min(trits_t x, trits_t s, trits_t y) {
  size_t n;
  MAM2_ASSERT(trits_size(x) == trits_size(y));
  n = min(trits_size(x), trits_size(s));
  trits_copy_add(trits_take(x, n), trits_take(s, n), trits_take(y, n));
  return n;
}

size_t trits_copy_sub_min(trits_t x, trits_t s, trits_t y) {
  size_t n;
  MAM2_ASSERT(trits_size(x) == trits_size(y));
  n = min(trits_size(x), trits_size(s));
  trits_copy_sub(trits_take(x, n), trits_take(s, n), trits_take(y, n));
  return n;
}

size_t trits_swap_add_min(trits_t x, trits_t s) {
  size_t n;
  n = min(trits_size(x), trits_size(s));
  trits_swap_add(trits_take(x, n), trits_take(s, n));
  return n;
}

size_t trits_swap_sub_min(trits_t x, trits_t s) {
  size_t n;
  n = min(trits_size(x), trits_size(s));
  trits_swap_sub(trits_take(x, n), trits_take(s, n));
  return n;
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

bool_t trits_cmp_eq_str(trits_t x, char const *y) {
  bool_t r = 1;

  for (; r && !trits_is_empty(x); x = trits_drop_min(x, 3))
    r = (trits_get_char(x) == *y++) && r;

  return r;
}

bool_t trits_is_same(trits_t x, trits_t y) {
  return (x.p == y.p) && (x.d == y.d); /* && (x.n == y.n) */
}

bool_t trits_is_overlapped(trits_t x, trits_t y) {
  word_t *x_first = x.p + (x.d / MAM2_TRITS_PER_WORD);
  word_t *x_last =
      x.p + (x.n / MAM2_TRITS_PER_WORD) + ((x.n % MAM2_TRITS_PER_WORD) ? 1 : 0);
  word_t *y_first = y.p + (y.d / MAM2_TRITS_PER_WORD);
  word_t *y_last =
      y.p + (y.n / MAM2_TRITS_PER_WORD) + ((y.n % MAM2_TRITS_PER_WORD) ? 1 : 0);
  return (x_first < y_last) && (y_first < x_last);
}

trits_t trits_diff(trits_t begin, trits_t end) {
  MAM2_ASSERT(trits_is_overlapped(begin, end) || trits_is_empty(end));
  MAM2_ASSERT(begin.p == end.p);
  MAM2_ASSERT(begin.n == end.n);
  MAM2_ASSERT(begin.d <= end.d);

  return trits_from_rep(end.d - begin.d, begin.p);
}

trits_t trits_null() { return trits_from_rep(0, 0); }

bool_t trits_is_null(trits_t x) { return (0 == x.p); }

bool_t trits_inc(trits_t x) {
  trit_t t;
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) {
    t = trit_add(trits_get1(x), 1);
    trits_put1(x, t);
    if (0 != t) return 1;
  }
  return 0;
}

trits_t trits_alloc(ialloc *a, size_t n) {
  word_t *p = malloc(MAM2_WORDS(n));
  return trits_from_rep(n, p);
}

void trits_free(ialloc *a, trits_t x) {
  if (x.p) {
    free(x.p);
  }
}

void trits_print(trits_t x) {
  for (; !trits_is_empty(x); x = trits_drop_min(x, 3))
    printf("%c", trits_get_char(x));
}

void trits_print2(char const *pfx, trits_t x, char const *sfx) {
  if (pfx) printf("%s", pfx);
  trits_print(x);
  if (sfx) printf("%s", sfx);
}

static bool_t trits_test_trit() {
  bool_t r = 1;
  MAM2_TRITS_DEF0(x, MAM2_TRITS_PER_WORD);
  trit_t t0, t;
  size_t i;
  x = MAM2_TRITS_INIT(x, MAM2_TRITS_PER_WORD);

  trits_set_zero(x);
  for (i = 0; i < MAM2_TRITS_PER_WORD; ++i) {
    trits_put1(x, t0 = -1);
    t = trits_get1(x);
    r = r && (t == t0);

    trits_put1(x, t0 = 0);
    t = trits_get1(x);
    r = r && (t == t0);

    trits_put1(x, t0 = 1);
    t = trits_get1(x);
    r = r && (t == t0);

    x = trits_drop(x, 1);
  }

  return r;
}

static bool_t trits_test_trytes(trits_t x, char *s, char *t) {
  bool_t r = 1;
  size_t n = trits_size(x) / 3;
  trits_t y;
  trint1_t s1;
  trint3_t s3;
  trint9_t s9;
  trint18_t s18;

  MAM2_ASSERT(0 == (trits_size(x) % 3));

  trits_from_str(x, s);

  for (y = x; !trits_is_empty(y); y = trits_drop(y, 1)) {
    s1 = trits_get1(y);
    trits_put1(y, 0);
    trits_put1(y, s1);
    r = r && (s1 == trits_get1(y));
  }
  trits_to_str(x, t);
  r = r && (0 == memcmp(s, t, n));

  for (y = x; 3 <= trits_size(y); y = trits_drop(y, 3)) {
    s3 = trits_get3(y);
    trits_put3(y, 0);
    trits_put3(y, s3);
    r = r && (s3 == trits_get3(y));
  }
  trits_to_str(x, t);
  r = r && (0 == memcmp(s, t, n));

  for (y = x; 9 <= trits_size(y); y = trits_drop(y, 9)) {
    s9 = trits_get9(y);
    trits_put9(y, 0);
    trits_put9(y, s9);
    r = r && (s9 == trits_get9(y));
  }
  trits_to_str(x, t);
  r = r && (0 == memcmp(s, t, n));

  for (y = x; 18 <= trits_size(y); y = trits_drop(y, 18)) {
    s18 = trits_get18(y);
    trits_put18(y, 0);
    trits_put18(y, s18);
    r = r && (s18 == trits_get18(y));
  }
  trits_to_str(x, t);
  r = r && (0 == memcmp(s, t, n));

  return r;
}

static bool_t trits_test_add_sub() {
  bool_t r = 1;
  trit_t x, x1, x2, s1, s2, y1, y2;
  size_t ix, is;
  trit_t const ts[3] = {-1, 0, 1};

  for (ix = 0; ix < 3; ++ix)
    for (is = 0; is < 3; ++is) {
      x = ts[ix];

      s1 = ts[is];
      y1 = trit_add(x, s1);
      s1 = x;

      y2 = x;
      s2 = ts[is];
      trit_swap_add(&y2, &s2);

      r = r && (s1 == s2) && (y1 == y2);

      s1 = ts[is];
      x1 = trit_sub(y1, s1);
      s1 = x1;

      x2 = y2;
      s2 = ts[is];
      trit_swap_sub(&x2, &s2);

      r = r && (s1 == s2) && (x == x1) && (x == x2);
    }

  return r;
}

static bool_t trits_test_bytes() {
  bool_t r = 1;
  MAM2_TRITS_DEF0(x0, 5 * 3);
  MAM2_TRITS_DEF0(y0, 5 * 3);
  trits_t x, y;
  byte b[3];
  size_t n, k;
  x0 = MAM2_TRITS_INIT(x0, 5 * 3);
  y0 = MAM2_TRITS_INIT(y0, 5 * 3);

  for (n = 0; r && n < 11; ++n) {
    x = trits_take(x0, n);
    y = trits_take(y0, n);
    trits_set_zero(x);
    k = 0;
    do {
      trits_to_bytes(x, b);
      trits_set_zero(y);
      r = r && trits_from_bytes(y, b);
      r = r && trits_cmp_eq(x, y);
      k++;
    } while (r && trits_inc(x));
  }

  return r;
}

bool_t trits_test() {
  bool_t r = 1;
  char s[6 * 4 + 1] = "KLMNOPQABCDWXYZ9ZA9NZA9N";
  char t[6 * 4];
  size_t n;
  trits_t x;
  MAM2_TRITS_DEF0(y, 3 * 6 * 4);
  y = MAM2_TRITS_INIT(y, 3 * 6 * 4);

  r = r && trits_test_trit();

  for (n = 0; n <= 6 * 4; ++n) {
    x = trits_take(y, 3 * n);
    r = r && trits_test_trytes(x, s, t);
  }

  r = r && trits_test_add_sub();

  r = r && trits_test_bytes();

  return r;
}
