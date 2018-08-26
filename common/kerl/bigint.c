/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

#include "common/kerl/bigint.h"

typedef struct {
  uint8_t carry;
  uint32_t v;
} full_add_s;

static void full_add(full_add_s *const fa, uint64_t const lh,
                     uint64_t const rh) {
  uint8_t carry1;
  uint32_t hi;
  uint64_t v;

  v = lh + rh;
  hi = (uint32_t)(v >> 32uLL);
  fa->v = v & 0xFFFFFFFF;

  carry1 = hi != 0;
  if (fa->carry) {
    v = fa->v + 1uLL;
    hi = (uint32_t)(v >> 32uLL);
    fa->v = v & 0xFFFFFFFF;
    fa->carry = carry1 || (hi != 0);
  } else {
    fa->carry = carry1;
  }
}

void bigint_not(uint32_t *const base, size_t const len) {
  for (size_t i = len; i--;) {
    base[i] = ~base[i];
  }
}

size_t bigint_add_small(uint32_t *const base, uint32_t const other) {
  // Note that this method doesn't do any bounds checks!
  size_t i = 1;
  full_add_s fa = {0, 0};
  full_add(&fa, base[0], other);

  base[0] = fa.v;

  while (fa.carry) {
    full_add(&fa, base[i], 0);
    base[i] = fa.v;
    i++;
  }

  return i;
}

void bigint_add(uint32_t *const base, uint32_t const *const rh,
                size_t const len) {
  full_add_s fa = {0, 0};
  size_t i = 0;

  for (; i < len; i++) {
    full_add(&fa, base[i], rh[i]);
    base[i] = fa.v;
  }
}

void bigint_sub(uint32_t *const base, uint32_t const *const rh,
                size_t const len) {
  full_add_s fa = {1, 0};
  size_t i = 0;

  for (; i < len; i++) {
    full_add(&fa, base[i], ~rh[i]);
    base[i] = fa.v;
  }

  assert(fa.carry);
}

int8_t bigint_cmp(uint32_t const *const lh, uint32_t const *const rh,
                  size_t const len) {
  size_t i = len;

  for (; i-- > 0;) {
    if (lh[i] < rh[i]) {
      return -1;
    } else if (lh[i] > rh[i]) {
      return 1;
    }
  }
  return 0;
}
