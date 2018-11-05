
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/alloc.h"
#include "mam/v2/buffers.h"
#include "mam/v2/curl.h"
#include "mam/v2/mam.h"
#include "mam/v2/mss.h"
#include "mam/v2/ntru.h"
#include "mam/v2/pb3.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/trits.h"
#include "mam/v2/wots.h"

#include <string.h>
#include <unity/unity.h>

#include <memory.h>
#include <stdio.h>

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

  TEST_ASSERT(r);
}

static void trits_test_add_sub() {
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

  TEST_ASSERT(r);
}

static void trits_test_bytes() {
  bool_t r = 1;
  MAM2_TRITS_DEF(x0, 5 * 3);
  MAM2_TRITS_DEF(y0, 5 * 3);
  trits_t x, y;
  byte b[3];
  size_t n, k;

  for (n = 0; r && n < 11; ++n) {
    x = trits_take(x0, n);
    y = trits_take(y0, n);
    trits_set_zero(x);
    k = 0;
    do {
      trits_to_bytes(x, b);
      trits_set_zero(y);
      r = r && trits_from_bytes(y, b);
      r = r && (0 == trits_cmp_grlex(x, y));
      k++;
    } while (r && trits_inc(x));
  }

  TEST_ASSERT(r);
}

static void trits_test() {
  bool_t r = 1;
  MAM2_TRITS_DEF(y, 3 * 6 * 4);
  trits_t x;
  char s[6 * 4 + 1] = "KLMNOPQABCDWXYZ9ZA9NZA9N";
  char t[6 * 4];
  size_t n;

  for (n = 0; n <= 6 * 4; ++n) {
    x = trits_take(y, 3 * n);
    // TEST_ASSERT(trits_test_trytes(x, s, t));
  }

  trits_test_add_sub();
  trits_test_bytes();
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(trits_test);
  RUN_TEST(trits_test_bytes);
  RUN_TEST(trits_test_add_sub);

  return UNITY_END();
}
