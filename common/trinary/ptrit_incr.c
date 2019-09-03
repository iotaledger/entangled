/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ptrit_incr.h"

static void trits_inc(size_t n, trit_t *t) {
  for (; n--;)
    if (2 == ++*t)
      *t++ = -1;
    else
      break;
}

size_t ptrit_log3(size_t n) {
  size_t k = 0, g = 1;
  for (; g < n; ++k, g *= 3)
    ;
  return k;
}

void ptrit_set_iota(size_t n, ptrit_t *range, trit_t *value) {
  size_t i;
  for (i = 0; i < PTRIT_SIZE; ++i) {
    ptrits_set_slice(n, range, i, value);
    trits_inc(n, value);
  }
}

int ptrit_hincr(size_t n, ptrit_t *t) {
  ptrit_s a, carry;
  for (; n; ++t, --n) {
#if defined(PTRIT_CVT_ANDN)
    // -1 -> (1,0); 0 -> (1,1); +1 -> (0,1); NaT -> (0,0)
    //   t    t+1  carry
    // (1,0) (1,1)  0
    // (1,1) (0,1)  0
    // (0,1) (1,0)  1
    // (0,0) (0,0)  0
    a = t->low;
    t->low = XOR(t->low, t->high);
    t->high = a;
    carry = ANDN(a, t->low);
#elif defined(PTRIT_CVT_ORN)
    // -1 -> (0,0); 0 -> (0,1); +1 -> (1,1); NaT -> (1,0)
    //   t    t+1  carry
    // (0,0) (0,1)  0
    // (0,1) (1,1)  0
    // (1,1) (0,0)  1
    // (1,0) (1,0)  0
    a = t->low;
    t->low = XOR(t->low, t->high);
    carry = AND(a, t->high);
    t->high = NOT(a);
#else
#error Invalid PTRIT_CVT.
#endif  // PTRIT_CVT
    if (0 == *(char *)&carry) break;
  }

  return (0 == n) ? 1 : 0;
}
