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

static bool pb3_sizet_test(size_t n) {
  retcode_t e;
  bool ok = true;
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

  return (e == RC_OK && ok && m == n);
}

static void pb3_sizets_test(void) {
  size_t n, k;

  n = 0;
  for (k = 0; k < 1000; ++k) {
    TEST_ASSERT_TRUE(pb3_sizet_test(n++));
  };

  if ((5 * sizeof(size_t) + 2) / 3 < 14) {
    for (n = 1, k = 0; k < sizeof(size_t); ++k) {
      n *= 243;
    }
    n -= 50;
    for (k = 0; k < 100; ++k) {
      TEST_ASSERT_TRUE(pb3_sizet_test(n++));
    }

    n = (size_t)0 - 100;
    for (k = 0; k < 100; ++k) {
      TEST_ASSERT_TRUE(pb3_sizet_test(n++));
    }
  }

  /* n = 2026277576509488133ull; (27^13-1)/2 */
  /*n = SIZE_MAX;*/
  n = 2026277576509488133ull;
  n -= 99;
  for (k = 0; k < 100; ++k) {
    TEST_ASSERT_TRUE(pb3_sizet_test(n++));
  };
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(pb3_sizets_test);

  return UNITY_END();
}
