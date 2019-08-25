/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <stdlib.h>

#include "common/defs.h"
#include "common/trinary/trit_long.h"
#include "mam/trits/trits.h"

static char const trinary_alphabet[] = "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"; /* [-13..13] */

bool tryte_from_char(tryte_t *t, char c) {
  bool r = true;

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
  MAM_ASSERT((tryte_t)-13 <= t && t <= (tryte_t)13);

  return trinary_alphabet[t + 13];
}

/*! \brief Minimum of two values. */
static size_t min(size_t k, size_t n) { return k < n ? k : n; }

bool trits_is_empty(trits_t x) { return (x.n == x.d); }

size_t trits_size(trits_t x) { return (x.n - x.d); }

size_t trits_size_min(trits_t x, size_t s) {
  size_t n;
  n = min(trits_size(x), s);
  return n;
}

trits_t trits_from_rep(size_t const n, trit_t const *const t) {
  trits_t x;
  x.n = n;
  x.d = 0;
  x.p = (trit_t *)t;
  return x;
}

trits_t trits_take(trits_t x, size_t n) {
  MAM_ASSERT(x.n >= x.d + n);
  x.n = x.d + n;
  return x;
}

trits_t trits_take_min(trits_t x, size_t n) {
  x.n = min(x.n, x.d + n);
  return x;
}

trits_t trits_drop(trits_t x, size_t n) {
  MAM_ASSERT(x.n >= x.d + n);
  x.d += n;
  return x;
}

trits_t trits_drop_min(trits_t x, size_t n) {
  x.d = min(x.n, x.d + n);
  return x;
}

trits_t trits_pickup(trits_t x, size_t n) {
  MAM_ASSERT(x.d >= n);
  x.d -= n;
  return x;
}

trits_t trits_pickup_all(trits_t x) {
  x.d = 0;
  return x;
}

trits_t trits_dropped_all(trits_t x) {
  x.n = x.d;
  return trits_pickup_all(x);
}

trits_t trits_advance(trits_t *b, size_t n) {
  trits_t t;
  MAM_ASSERT(b);
  t = trits_take(*b, n);
  *b = trits_drop(*b, n);
  return t;
}

static size_t trits_size_t_trytes(size_t const n) {
  MAM_ASSERT(n <= MAM_TRITS_SIZE_MAX);

  size_t const max_d = MAM_TRITS_MAX_SIZE_T_TRYTES;
  size_t d = 0, m = 1;

  for (; d < max_d && (n > (m - 1) / 2);) {
    m *= TRYTE_SPACE_SIZE;
    ++d;
  }

  return d;
}

size_t trits_sizeof_size_t(size_t n) {
  size_t d = trits_size_t_trytes(n);
  MAM_ASSERT(d <= MAM_TRITS_MAX_SIZE_T_TRYTES);

  /* one extra tryte to encode `d` */
  return 3 * (d + 1);
}

void trits_encode_size_t(size_t n, trits_t *const buffer) {
  MAM_ASSERT(buffer && !(trits_size(*buffer) < trits_sizeof_size_t(n)));

  trits_set_zero(*buffer);

  size_t d = trits_size_t_trytes(n);
  MAM_ASSERT(d <= MAM_TRITS_MAX_SIZE_T_TRYTES);

  trits_put3(trits_advance(buffer, 3), (tryte_t)d);

  if (n > 0) {
    long_to_trits(n, trits_begin(*buffer));
    trits_advance(buffer, d * NUMBER_OF_TRITS_IN_A_TRYTE);
  }
}

retcode_t trits_decode_size_t(size_t *const n, trits_t *const buffer) {
  MAM_ASSERT(n != 0);

  tryte_t d;
  uint64_t size;

  if (trits_size(*buffer) < NUMBER_OF_TRITS_IN_A_TRYTE) {
    return RC_MAM_PB3_EOF;
  }

  d = trits_get3(*buffer);
  *buffer = trits_drop(*buffer, NUMBER_OF_TRITS_IN_A_TRYTE);
  if (d < 0 || d > 13) {
    return RC_MAM_INVALID_VALUE;
  }
  if (trits_size(*buffer) < NUMBER_OF_TRITS_IN_A_TRYTE * (size_t)d) {
    return RC_MAM_PB3_EOF;
  }

  size = trits_to_long(trits_begin(*buffer), d * NUMBER_OF_TRITS_IN_A_TRYTE);
  if (size != (size_t)size) {
    return RC_MAM_TRITS_SIZE_T_NOT_SUPPORTED;
  }
  *n = size;
  trits_advance(buffer, d * NUMBER_OF_TRITS_IN_A_TRYTE);

  return RC_OK;
}

trint1_t trits_get1(trits_t x) {
  MAM_ASSERT(!trits_is_empty(x));
  MAM_ASSERT_TRINT1(x.p[x.d]);

  return x.p[x.d];
}

void trits_put1(trits_t x, trint1_t t) {
  MAM_ASSERT(!trits_is_empty(x));
  MAM_ASSERT_TRINT1(t);

  x.p[x.d] = t;
}

trint3_t trits_get3(trits_t x) {
  trint3_t t0, t1, t2;
  MAM_ASSERT(trits_size(x) >= 3);

  t0 = (trint3_t)trits_get1(x);
  t1 = (trint3_t)trits_get1(trits_drop(x, 1));
  t2 = (trint3_t)trits_get1(trits_drop(x, 2));
  return t0 + (trint3_t)3 * t1 + (trint3_t)9 * t2;
}

void trits_put3(trits_t x, trint3_t t) {
  trint1_t t0, t1, t2;
  MAM_ASSERT(-13 <= t && t <= 13);
  MAM_ASSERT(trits_size(x) >= 3);

  t0 = MAM_MODS(t, 3 * 3 * 3, 3);
  t = MAM_DIVS(t, 3 * 3 * 3, 3);
  t1 = MAM_MODS(t, 3 * 3, 3);
  t = MAM_DIVS(t, 3 * 3, 3);
  t2 = (trint1_t)t;

  trits_put1(x, t0);
  trits_put1(trits_drop(x, 1), t1);
  trits_put1(trits_drop(x, 2), t2);
}

trint6_t trits_get6(trits_t x) {
  trint6_t t0, t1;
  MAM_ASSERT(trits_size(x) >= 6);

  t0 = (trint6_t)trits_get3(x);
  t1 = (trint6_t)trits_get3(trits_drop(x, 3));
  return t0 + (trint6_t)27 * t1;
}

void trits_put6(trits_t x, trint6_t t) {
  trint3_t t0, t1;
  MAM_ASSERT(-364 <= t && t <= 364);
  MAM_ASSERT(trits_size(x) >= 6);

  t0 = MAM_MODS(t, 27 * 27, 27);
  t = MAM_DIVS(t, 27 * 27, 27);
  t1 = (trint3_t)t;

  trits_put3(x, t0);
  trits_put3(trits_drop(x, 3), t1);
}

trint9_t trits_get9(trits_t x) {
  trint9_t t0, t1, t2;
  MAM_ASSERT(trits_size(x) >= 9);

  t0 = (trint9_t)trits_get3(x);
  t1 = (trint9_t)trits_get3(trits_drop(x, 3));
  t2 = (trint9_t)trits_get3(trits_drop(x, 6));
  return t0 + (trint9_t)27 * t1 + (trint9_t)729 * t2;
}

void trits_put9(trits_t x, trint9_t t) {
  trint3_t t0, t1, t2;
  MAM_ASSERT(-9841 <= t && t <= 9841);
  MAM_ASSERT(trits_size(x) >= 9);

  t0 = MAM_MODS(t, 27 * 27 * 27, 27);
  t = MAM_DIVS(t, 27 * 27 * 27, 27);
  t1 = MAM_MODS(t, 27 * 27, 27);
  t = MAM_DIVS(t, 27 * 27, 27);
  t2 = (trint3_t)t;

  trits_put3(x, t0);
  trits_put3(trits_drop(x, 3), t1);
  trits_put3(trits_drop(x, 6), t2);
}

trint18_t trits_get18(trits_t x) {
  trint18_t t0, t1;
  MAM_ASSERT(trits_size(x) >= 18);

  t0 = (trint18_t)trits_get9(x);
  t1 = (trint18_t)trits_get9(trits_drop(x, 9));
  return t0 + (trint18_t)19683 * t1;
}

void trits_put18(trits_t x, trint18_t t) {
  trint9_t t0, t1;
  MAM_ASSERT(trits_size(x) >= 18);

  t0 = MAM_MODS(t, 19683 * 19683, 19683);
  t = MAM_DIVS(t, 19683 * 19683, 19683);
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

bool trits_put_char(trits_t x, char c) {
  trit_t ts[3] = {0, 0, 0};
  tryte_t i = 0;
  size_t k;

  if (!tryte_from_char(&i, c)) return 0;

  ts[0] = MAM_MODS(i, 3 * 3 * 3, 3);
  i = MAM_DIVS(i, 3 * 3 * 3, 3);
  ts[1] = MAM_MODS(i, 3 * 3, 3);
  i = MAM_DIVS(i, 3 * 3, 3);
  ts[2] = MAM_MODS(i, 3, 3);

  for (k = trits_size_min(x, 3); k < 3; ++k)
    if (0 != ts[k]) return 0;

  trits_put(trits_take_min(x, 3), ts);
  return 1;
}

void trits_to_str(trits_t x, char *s) {
  for (; !trits_is_empty(x); x = trits_drop_min(x, 3)) *s++ = trits_get_char(x);
}

bool trits_from_str(trits_t x, char const *s) {
  bool r = true;

  for (; r && !trits_is_empty(x); x = trits_drop_min(x, 3)) r = trits_put_char(x, *s++);

  return r;
}

size_t trits_copy_min(trits_t x, trits_t y) {
  size_t n;
  n = min(trits_size(x), trits_size(y));
  trits_copy(trits_take(x, n), trits_take(y, n));
  return n;
}

void trits_padc0(trit_t c0, trits_t y) {
  MAM_ASSERT(!trits_is_empty(y));

  /* c0 */
  trits_put1(y, c0);
  /* 0* */
  trits_set_zero(trits_drop(y, 1));
}

void trits_padc(trit_t c0, trits_t y) {
  if (!trits_is_empty(y)) trits_padc0(c0, y);
}

#define MAM_TRIT_NORM(t) (((t) == 2) ? -1 : ((t) == -2) ? 1 : (t))
trit_t trit_add(trit_t x, trit_t s) {
  trit_t y = x + s;
  y = MAM_TRIT_NORM(y);
  return y;
}
trit_t trit_sub(trit_t y, trit_t s) {
  trit_t x = y - s;
  x = MAM_TRIT_NORM(x);
  return x;
}
void trit_swap_add(trit_t *x, trit_t *s) {
  trit_t y = *x + *s;
  y = MAM_TRIT_NORM(y);
  *s = *x;
  *x = y;
}
void trit_swap_sub(trit_t *y, trit_t *s) {
  trit_t x = *y - *s;
  x = MAM_TRIT_NORM(x);
  *s = x;
  *y = x;
}

void trits_copy_add(trits_t x, trits_t s, trits_t y) {
  trit_t tx, ts, ty;

  MAM_ASSERT(trits_size(x) == trits_size(s));
  MAM_ASSERT(trits_size(x) == trits_size(y));

  for (; !trits_is_empty(x); x = trits_drop(x, 1), s = trits_drop(s, 1), y = trits_drop(y, 1)) {
    tx = trits_get1(x);
    ts = trits_get1(s);
    ty = trit_add(tx, ts);
    trits_put1(s, tx);
    trits_put1(y, ty);
  }
}

void trits_copy_sub(trits_t y, trits_t s, trits_t x) {
  trit_t tx, ts, ty;

  MAM_ASSERT(trits_size(x) == trits_size(s));
  MAM_ASSERT(trits_size(x) == trits_size(y));

  for (; !trits_is_empty(x); x = trits_drop(x, 1), s = trits_drop(s, 1), y = trits_drop(y, 1)) {
    ty = trits_get1(y);
    ts = trits_get1(s);
    tx = trit_sub(ty, ts);
    trits_put1(s, tx);
    trits_put1(x, tx);
  }
}

void trits_swap_add(trits_t x, trits_t s) {
  trit_t tx, ts;

  MAM_ASSERT(trits_size(x) == trits_size(s));

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

  MAM_ASSERT(trits_size(y) == trits_size(s));

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
  MAM_ASSERT(trits_size(x) == trits_size(y));
  n = min(trits_size(x), trits_size(s));
  trits_copy_add(trits_take(x, n), trits_take(s, n), trits_take(y, n));
  return n;
}

size_t trits_copy_sub_min(trits_t x, trits_t s, trits_t y) {
  size_t n;
  MAM_ASSERT(trits_size(x) == trits_size(y));
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

  for (; !trits_is_empty(x) && (0 == (d = trits_get1(x) - trits_get1(y))); x = trits_drop(x, 1), y = trits_drop(y, 1))
    ;

  return d;
}

bool trits_cmp_eq(trits_t x, trits_t y) { return (0 == trits_cmp_grlex(x, y)) ? 1 : 0; }

bool trits_cmp_eq_str(trits_t x, char const *y) {
  bool r = true;

  for (; r && !trits_is_empty(x); x = trits_drop_min(x, 3)) r = (trits_get_char(x) == *y++) && r;

  return r;
}

trits_t trits_diff(trits_t begin, trits_t end) {
  MAM_ASSERT(begin.p == end.p);    // same buffer
  MAM_ASSERT(begin.d <= end.d);    // begin before end
  MAM_ASSERT(begin.d <= begin.n);  // begin consistent
  MAM_ASSERT(end.d <= end.n);      // end consistent

  begin.n = end.d;

  return begin;
}

trits_t trits_null() { return trits_from_rep(0, 0); }

bool trits_is_null(trits_t x) { return (0 == x.p); }

bool trits_inc(trits_t x) {
  trit_t t;
  for (; !trits_is_empty(x); x = trits_drop(x, 1)) {
    t = trit_add(trits_get1(x), 1);
    trits_put1(x, t);
    if (0 != t) return 1;
  }
  return 0;
}

trits_t trits_alloc(size_t n) {
  trit_t *p = (trit_t *)malloc(n);
  return trits_from_rep(n, p);
}

void trits_free(trits_t x) {
  if (x.p) {
    free(x.p);
  }
}
