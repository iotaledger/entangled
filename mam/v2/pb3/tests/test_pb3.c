/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "mam/v2/pb3/pb3.h"

static bool_t pb3_test_sizet(size_t n) {
  err_t e;
  bool_t ok = 1;
  size_t k = pb3_sizeof_sizet(n);
  size_t m = 0;
  MAM2_TRITS_DEF0(b0, 3 * 14); /* 14 trytes max */
  trits_t b;
  b0 = MAM2_TRITS_INIT(b0, 3 * 14); /* 14 trytes max */

  b = trits_take(b0, k);
  pb3_encode_sizet(n, &b);
  ok = ok && trits_is_empty(b);

  b = trits_take(b0, k);
  e = pb3_decode_sizet(&m, &b);
  ok = ok && trits_is_empty(b);

  return (e == err_ok && ok && m == n);
}

static bool_t pb3_test_sizets() {
  bool_t r = 1;

  size_t n, k;

  n = 0;
  for (k = 0; r && k < 1000; ++k) r = pb3_test_sizet(n++);

  if ((5 * sizeof(size_t) + 2) / 3 < 14) {
    for (n = 1, k = 0; k < sizeof(size_t); ++k) n *= 243;
    n -= 50;
    for (k = 0; r && k < 100; ++k) r = pb3_test_sizet(n++);

    n = (size_t)0 - 100;
    for (k = 0; r && k < 100; ++k) r = pb3_test_sizet(n++);
  }

  /* n = 2026277576509488133ull; (27^13-1)/2 */
  /*n = SIZE_MAX;*/
  n = 2026277576509488133ull;
  n -= 99;
  for (k = 0; r && k < 100; ++k) r = pb3_test_sizet(n++);

  return r;
}

bool_t pb3_test() {
  bool_t r = 1;
  r = r && pb3_test_sizets();
  /*TODO*/
  return r;
}

int main(void) {
  UNITY_BEGIN();

  return UNITY_END();
}
