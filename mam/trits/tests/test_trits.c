/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include <unity/unity.h>

#include "mam/trits/trits.h"

static void trits_put_get_test(void) {
  trit_t t0, t;
  size_t i;

  MAM_TRITS_DEF(x, 1);

  x = MAM_TRITS_INIT(x, 1);
  trits_set_zero(x);

  for (i = 0; i < 1; ++i) {
    trits_put1(x, t0 = -1);
    t = trits_get1(x);
    TEST_ASSERT_TRUE(t == t0);

    trits_put1(x, t0 = 0);
    t = trits_get1(x);
    TEST_ASSERT_TRUE(t == t0);

    trits_put1(x, t0 = 1);
    t = trits_get1(x);
    TEST_ASSERT_TRUE(t == t0);

    x = trits_drop(x, 1);
  }
}

static void trits_add_sub_test(void) {
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

      TEST_ASSERT_TRUE(s1 == s2);
      TEST_ASSERT_TRUE(y1 == y2);

      s1 = ts[is];
      x1 = trit_sub(y1, s1);
      s1 = x1;

      x2 = y2;
      s2 = ts[is];
      trit_swap_sub(&x2, &s2);

      TEST_ASSERT_TRUE(s1 == s2);
      TEST_ASSERT_TRUE(x == x1);
      TEST_ASSERT_TRUE(x == x2);
    }
}

static bool trits_trytes(trits_t x, char *s, char *t) {
  bool r = true;
  size_t n = trits_size(x) / 3;
  trits_t y;
  trint1_t s1;
  trint3_t s3;
  trint9_t s9;
  trint18_t s18;

  MAM_ASSERT(0 == (trits_size(x) % 3));

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

static void trits_test(void) {
  char s[6 * 4 + 1] = "KLMNOPQABCDWXYZ9ZA9NZA9N";
  char t[6 * 4];
  trits_t x;

  MAM_TRITS_DEF(y, 3 * 6 * 4);
  y = MAM_TRITS_INIT(y, 3 * 6 * 4);

  for (size_t n = 0; n <= 6 * 4; ++n) {
    x = trits_take(y, 3 * n);
    TEST_ASSERT_TRUE(trits_trytes(x, s, t));
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(trits_put_get_test);
  RUN_TEST(trits_add_sub_test);
  RUN_TEST(trits_test);

  return UNITY_END();
}
