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

#include "common/trinary/trit_long.h"
#include "mam/v2/sponge.h"
#include "mam/v2/tests/common.h"
#include "mam/v2/trits.h"
#include "utils/macros.h"

#include <string.h>
#include <unity/unity.h>

#include <memory.h>
#include <stdio.h>

static void sponge_test_hash(sponge_t *s) {
  trit_array_t xs_flex_trits[3];
  size_t n;
  TRIT_ARRAY_DECLARE(y1_flex_trits, MAM2_SPONGE_HASH_SIZE);
  TRIT_ARRAY_DECLARE(y2_flex_trits, MAM2_SPONGE_HASH_SIZE);

  for (n = 0; n < MAM2_SPONGE_RATE * 3; n += MAM2_SPONGE_RATE / 2) {
    TRIT_ARRAY_DECLARE(x_flex_trits, n);
    trit_array_set_null(&x_flex_trits);

    sponge_hash(s, &x_flex_trits, &y1_flex_trits);

    TRIT_ARRAY_DECLARE(xs_0, n / 3);
    xs_flex_trits[0] = xs_0;

    TRIT_ARRAY_DECLARE(xs_1, n / 4);
    xs_flex_trits[1] = xs_1;

    TRIT_ARRAY_DECLARE(xs_2, n - n / 3 - n / 4);
    xs_flex_trits[2] = xs_2;

    sponge_hashn(s, 3, xs_flex_trits, &y2_flex_trits);
    TEST_ASSERT(0 == memcmp(y1_flex_trits.trits, y2_flex_trits.trits,
                            FLEX_TRIT_SIZE_243));
  }
}

static bool sponge_test_ae(sponge_t *s) {
#define MAM2_SPONGE_TEST_MAX_K 1110
  size_t k, i;
  TRIT_ARRAY_DECLARE(K, MAM2_SPONGE_KEY_SIZE);
  TRIT_ARRAY_DECLARE(X, MAM2_SPONGE_TEST_MAX_K);
  TRIT_ARRAY_DECLARE(Y, MAM2_SPONGE_TEST_MAX_K);
  TRIT_ARRAY_DECLARE(Z, MAM2_SPONGE_TEST_MAX_K);
  size_t ks[] = {0,
                 1,
                 2,
                 3,
                 4,
                 5,
                 6,
                 242,
                 243,
                 244,
                 485,
                 486,
                 487,
                 MAM2_SPONGE_TEST_MAX_K,
                 MAM2_SPONGE_TEST_MAX_K + 1};

  // init X
  trit_t tmp[3];
  for (i = 0; i < MAM2_SPONGE_TEST_MAX_K / 3; ++i) {
    trit_t val = (tryte_t)((i % 27) - 13);
    long_to_trits(val, tmp);
    trit_array_set_at(&X, 3 * i, tmp[0]);
    trit_array_set_at(&X, 3 * i + 1, tmp[1]);
    trit_array_set_at(&X, 3 * i + 2, tmp[2]);
  }
  // init K
  trit_array_set_null(&K);
  flex_trits_from_trytes(K.trits, 81,
                         "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                         "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                         "NOPQRSTUVWXYZ9ABCDEFGHIJKLM",
                         81, 81);

  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, &K);
  sponge_squeeze_flex(s, MAM2_SPONGE_CTL_KEY, &K);

  for (i = 0; (k = ks[i++]) <= MAM2_SPONGE_TEST_MAX_K;) {
    TRIT_ARRAY_DECLARE(curr_X, k);
    TRIT_ARRAY_DECLARE(curr_Y, k);
    TRIT_ARRAY_DECLARE(curr_Z, k);
    trit_array_set_trits(&curr_X, X.trits, k);
    trit_array_set_trits(&curr_Y, Y.trits, k);
    trit_array_set_trits(&curr_Z, Z.trits, k);
    sponge_init(s);
    sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, &K);
    sponge_encr_flex(s, &curr_X, &curr_Y);  // Y=E(X)
    sponge_init(s);
    sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, &K);
    sponge_decr_flex(s, &curr_Y, &curr_Z);  // Z=D(E(X))
    if (!trit_array_is_equal(&curr_X, &curr_Z)) {
      return false;
    }
    sponge_init(s);
    sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, &K);
    sponge_encr_flex(s, &curr_Z, &curr_Z);  // Z=E(Z=X)
    if (!trit_array_is_equal(&curr_Y, &curr_Z)) {
      return false;
    }
    sponge_init(s);
    sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, &K);
    sponge_decr_flex(s, &curr_Z, &curr_Z);  // Z=D(Z=E(X))
    if (!trit_array_is_equal(&curr_X, &curr_Z)) {
      return false;
    }
  }

#undef MAM2_SPONGE_TEST_MAX_K
  return true;
}

static void sponge_test_pointwise(sponge_t *s) {
  bool r, ok;

  MAM2_TRITS_DEF(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF(X, 2 * MAM2_SPONGE_RATE + 3);
  MAM2_TRITS_DEF(Y, 2 * MAM2_SPONGE_RATE + 3);
  MAM2_TRITS_DEF(T, 2 * MAM2_SPONGE_RATE + 3);
  trits_t x, y, t;

  char const rnd[(MAM2_SPONGE_KEY_SIZE + 2 * MAM2_SPONGE_RATE + 3) / 3 + 1] =
      "SERQWES9QWEFYSDKVJKPOKWFJKL"
      "JD9QW9EAWZMAJLQKJWEO9SDFPWA"
      "LKDIXNKISDFJ9ENKMLPQIOMLZQE"

      "SDFLJKWERQWESDIFYSDKVJKPOKL"
      "JZMAJLQKJWEO9SDFQPPQIOWEAMZ"
      "XNKISDF9QWELKJ99QW9EAWENKML"
      "QWPEOIERTYIASDLKJZXCMNSDF9N"
      "K9ASDPLZXCMSDFWEROIUCXVJKSF"
      "9REWRVXCVMAEKRUOIHEWLZXCPVS"

      "FLJSDKWEDIFYKVJKLKNWELKJ99Q"
      "JZMAJLQKRQWESSDFQPPQIOWEAMZ"
      "JZXCMNSXISDF9QW9EAWUOIHEEPO"
      "NKQWPETYIASDLKDFSSJWE9NKMLR"
      "RVXCVMAK9ASRLZXC9DDFWEOIUCX"
      "9REWEKRWLZXDPOIEMOCPVSVJKSF"

      "Z";
  char const hash_y[MAM2_SPONGE_HASH_SIZE / 3 + 1] =
      "99B9WYVWFGZVQTFAQ9NLWNNFYDJ"
      "AIGVWKROPLKSXS9TAPIGVUBHBZJ"
      "KRMLVUHKYZIIVQ9GTQSIUXXCSYS";
  char const encr_y[(2 * MAM2_SPONGE_RATE + 3) / 3 + 1] =
      "SDHFADDVJQPMXJGYZCNUXWZ9TQV"
      "UKTBJBPKOXMNMQKRJ9WPNLWXBSO"
      "BONTTMLNDEXGQQ9EGJMQNPNKOPK"
      "JMNEOIERTYIASDLKJZXCMNSDF9N"
      "K9ASDPLZXCMSDFWEROIUCXVJKSF"
      "9REWRVXCVMAEKRUOIHEWLZXCPVS"
      "ADEENUZFXEJRXGPZAFNXQFYRZWC"
      "WMEDUV9LADHFTXFPJTGZUNHRTG9"
      "HEFVXMWJWDWSUQUFP9HMRLC9BRN"
      "LTJDKVHGNSLZULTJDCQYKC9ABGX"
      "RIHCTBB9L9PUPRAIWIVCEZRFGMH"
      "SXENVJIRIBSENBTWYLLX9OYIHVD"
      "R";
  char const decr_y[(2 * MAM2_SPONGE_RATE + 3) / 3 + 1] =
      "SDMISU99ZQFIKPKNCEIDHUNKSWB"
      "WNCXJVUKBYIPNRUC9KFUD9WPXYM"
      "APZYOVIMFNPHW99VCUNBLFWZAAM"
      "9ROEOIERTYIASDLKJZXCMNSDF9N"
      "K9ASDPLZXCMSDFWEROIUCXVJKSF"
      "9REWRVXCVMAEKRUOIHEWLZXCPVS"
      "TTBHLYTXYRCKIXIBUZRIOSBOPFL"
      "9YTJFXJCGFAETMCXJCOVGUASFZB"
      "OITLBKGBSCKKYUPDXGJGYRKLZHZ"
      "MDQEA9WPDJWXCMLKGE9XHXMOUHL"
      "RWVCUUXPOZVOZJBODNMRNTULZTV"
      "HLEE9F9MFWKROJYNS9UEETJHEPQ"
      "M";

  trytes_to_trits(rnd, K.p, MIN(strlen(rnd), K.n / RADIX));
  trytes_to_trits(rnd + MAM2_SPONGE_KEY_SIZE / 3, X.p,
                  strlen(rnd + MAM2_SPONGE_KEY_SIZE / 3));
  trits_set_zero(Y);

  sponge_init(s);
  x = X;
  sponge_absorb(s, MAM2_SPONGE_CTL_DATA, x);
  y = trits_take(Y, MAM2_SPONGE_HASH_SIZE);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, y);
  t = trits_take(T, MAM2_SPONGE_HASH_SIZE);
  trytes_to_trits(hash_y, t.p, strlen(hash_y));

  r = trits_cmp_eq(y, t);
  TEST_ASSERT(r);

  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  sponge_encr(s, x, y);

  t = T;
  trytes_to_trits(encr_y, t.p, strlen(encr_y));

  r = trits_cmp_eq(y, t);
  TEST_ASSERT(r);
  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  trits_copy(x, y);
  sponge_encr(s, y, y);
  t = T;
  trytes_to_trits(encr_y, t.p, strlen(encr_y));
  r = trits_cmp_eq(y, t);
  TEST_ASSERT(r);

  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  sponge_decr(s, x, y);
  t = T;

  trytes_to_trits(decr_y, t.p, strlen(decr_y));
  r = trits_cmp_eq(y, t);
  TEST_ASSERT(r);
  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  trits_copy(x, y);
  sponge_decr(s, y, y);
  t = T;
  trytes_to_trits(decr_y, t.p, strlen(decr_y));
  r = trits_cmp_eq(y, t);
  TEST_ASSERT(r);
}

void sponge_test(sponge_t *s) {
  sponge_test_ae(s);
  sponge_test_hash(s);
  sponge_test_pointwise(s);
}

int main(void) {
  UNITY_BEGIN();

  test_sponge_t _s[1];
  sponge_t *s = test_sponge_init(_s);

  sponge_test(s);

  return UNITY_END();
}
