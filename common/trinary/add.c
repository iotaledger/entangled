#include "common/trinary/add.h"

struct ha_o {
  trit_t s;
  trit_t c;
};

trit_t trit_cons(trit_t a, trit_t b) { return a == b ? a : 0; }

trit_t trit_sum(trit_t a, trit_t b) {
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
struct ha_o trit_full_add(trit_t a, trit_t b, trit_t c) {
  trit_t s_ab = trit_sum(a, b);
  return (struct ha_o){trit_sum(s_ab, c),
                       (((trit_cons(a, b) + trit_cons(s_ab, c)) > 0) << 1) - 1};
}

int add_internal(trit_t *const t, size_t const s, int8_t const n,
                 int64_t const v, struct ha_o r) {
  if (!v && !r.c) return 0;
  if (s == 0) return 1;

  trit_t trit;
  if (n) {
    trit = -(((v + 1) % 3) - 1);
  } else {
    trit = ((v + 1) % 3) - 1;
  }

  r = trit_full_add(*t, trit, r.c);
  *t = r.s;
  return add_internal(&t[1], s - 1, n, (v + 1) / 3, (struct ha_o){r.s, r.c});
}

int add_assign(trit_t *t, size_t s, int64_t v) {
  return add_internal(t, s, v < 0, v, (struct ha_o){0, 0});
}
