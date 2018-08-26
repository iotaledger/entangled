/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/add.h"

struct ha_o {
  trit_t s;
  trit_t c;
};

trit_t trit_cons(trit_t const a, trit_t const b) { return a == b ? a : 0; }

trit_t trit_sum(trit_t const a, trit_t const b) {
  trit_t s = a + b;
  switch (s) {
    case 2:
      return -1;
    case -2:
      return 1;
    default:
      return s;
  }
}

/// Adds values `a` with `b` with a carry `c`, and returns (sum, carry)
struct ha_o trit_full_add(trit_t const a, trit_t const b, trit_t const c) {
  trit_t s_ab = trit_sum(a, b);

  trit_t x = trit_cons(a, b) + trit_cons(s_ab, c);
  return (struct ha_o){trit_sum(s_ab, c), (x > 0) - (x < 0)};
}

int add_internal(trit_t *const t, size_t const s, int8_t const negative,
                 int64_t const v, struct ha_o r) {
  trit_t trit;

  if (!v && !r.c) return 0;
  if (s == 0) return 1;

  trit = ((v + 1) % 3) - 1;
  if (negative) {
    trit = -trit;
  }

  r = trit_full_add(*t, trit, r.c);
  *t = r.s;
  return add_internal(&t[1], s - 1, negative, (v + 1) / 3, r);
}

int add_assign(trit_t *const t, size_t const s, int64_t const v) {
  return add_internal(t, s, v < 0, v, (struct ha_o){0, 0});
}

void add_trits(trit_t const *const lh, trit_t *const rh, size_t const len) {
  struct ha_o r = {0, 0};
  size_t i = 0;
  for (; i < len; i++) {
    r = trit_full_add(lh[i], rh[i], r.c);
    rh[i] = r.s;
  }
}
