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

static void sponge_test_hash(sponge_t *const s) {
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

static void sponge_test_pointwise(sponge_t *const s) {
  TRIT_ARRAY_DECLARE(K, MAM2_SPONGE_KEY_SIZE);
  TRIT_ARRAY_DECLARE(X, 2 * MAM2_SPONGE_RATE + 3);
  TRIT_ARRAY_DECLARE(Y, 2 * MAM2_SPONGE_RATE + 3);
  TRIT_ARRAY_DECLARE(y, MAM2_SPONGE_HASH_SIZE);

  tryte_t *rnd_trytes = (tryte_t*)
      "SERQWES9QWEFYSDKVJKPOKWFJKLJD9QW9EAWZMAJLQKJWEO9SDFPWALKDIXNKISDFJ9ENKML"
      "PQIOMLZQESDFLJKWERQWESDIFYSDKVJKPOKLJZMAJLQKJWEO9SDFQPPQIOWEAMZXNKISDF9Q"
      "WELKJ99QW9EAWENKMLQWPEOIERTYIASDLKJZXCMNSDF9NK9ASDPLZXCMSDFWEROIUCXVJKSF"
      "9REWRVXCVMAEKRUOIHEWLZXCPVSFLJSDKWEDIFYKVJKLKNWELKJ99QJZMAJLQKRQWESSDFQP"
      "PQIOWEAMZJZXCMNSXISDF9QW9EAWUOIHEEPONKQWPETYIASDLKDFSSJWE9NKMLRRVXCVMAK9"
      "ASRLZXC9DDFWEOIUCX9REWEKRWLZXDPOIEMOCPVSVJKSFZ";

  tryte_t *hash_y_trytes = (tryte_t*)
      "99B9WYVWFGZVQTFAQ9NLWNNFYDJAIGVWKROPLKSXS9TAPIGVUBHBZJKRMLVUHKYZIIVQ9GTQ"
      "SIUXXCSYS";
  size_t hash_y_length = strlen((char *)hash_y_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(hash_y, hash_y_length, hash_y_trytes);

  tryte_t *encr_y_trytes =
      "SDHFADDVJQPMXJGYZCNUXWZ9TQVUKTBJBPKOXMNMQKRJ9WPNLWXBSOBONTTMLNDEXGQQ9EGJ"
      "MQNPNKOPKJMNEOIERTYIASDLKJZXCMNSDF9NK9ASDPLZXCMSDFWEROIUCXVJKSF9REWRVXCV"
      "MAEKRUOIHEWLZXCPVSADEENUZFXEJRXGPZAFNXQFYRZWCWMEDUV9LADHFTXFPJTGZUNHRTG9"
      "HEFVXMWJWDWSUQUFP9HMRLC9BRNLTJDKVHGNSLZULTJDCQYKC9ABGXRIHCTBB9L9PUPRAIWI"
      "VCEZRFGMHSXENVJIRIBSENBTWYLLX9OYIHVDR";
  size_t encr_y_length = strlen((char *)encr_y_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(encr_y, encr_y_length, encr_y_trytes);

  tryte_t *decr_y_trytes =
      "SDMISU99ZQFIKPKNCEIDHUNKSWBWNCXJVUKBYIPNRUC9KFUD9WPXYMAPZYOVIMFNPHW99VCU"
      "NBLFWZAAM9ROEOIERTYIASDLKJZXCMNSDF9NK9ASDPLZXCMSDFWEROIUCXVJKSF9REWRVXCV"
      "MAEKRUOIHEWLZXCPVSTTBHLYTXYRCKIXIBUZRIOSBOPFL9YTJFXJCGFAETMCXJCOVGUASFZB"
      "OITLBKGBSCKKYUPDXGJGYRKLZHZMDQEA9WPDJWXCMLKGE9XHXMOUHLRWVCUUXPOZVOZJBODN"
      "MRNTULZTVHLEE9F9MFWKROJYNS9UEETJHEPQM";
  size_t decr_y_length = strlen((char *)decr_y_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(decr_y, decr_y_length, decr_y_trytes);

  flex_trits_from_trytes(K.trits, MAM2_SPONGE_KEY_SIZE, rnd_trytes,
                         MAM2_SPONGE_KEY_SIZE / 3, MAM2_SPONGE_KEY_SIZE / 3);
  flex_trits_from_trytes(
      X.trits, 2 * MAM2_SPONGE_RATE + 3, rnd_trytes + MAM2_SPONGE_KEY_SIZE / 3,
      strlen((char *)(rnd_trytes + MAM2_SPONGE_KEY_SIZE / 3)),
      strlen((char *)(rnd_trytes + MAM2_SPONGE_KEY_SIZE / 3)));

  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_DATA, &X);
  sponge_squeeze_flex(s, MAM2_SPONGE_CTL_HASH, &y);
  TEST_ASSERT(trit_array_is_equal(&hash_y, &y));

  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, &K);
  sponge_encr_flex(s, &X, &Y);
  TEST_ASSERT(trit_array_is_equal(&encr_y, &Y));

  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, &K);
  memcpy(Y.trits, X.trits, X.num_bytes);
  sponge_encr_flex(s, &Y, &Y);
  TEST_ASSERT(trit_array_is_equal(&encr_y, &Y));

  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, &K);
  sponge_decr_flex(s, &Y, &X);
  TEST_ASSERT(trit_array_is_equal(&decr_y, &Y));

  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, &K);
  memcpy(Y.trits, X.trits, X.num_bytes);
  sponge_decr_flex(s, &Y, &Y);
  TEST_ASSERT(trit_array_equal(&decr_y, &Y));
}

int main(void) {
  UNITY_BEGIN();

  test_sponge_t _s;
  sponge_t *s = test_sponge_init(&_s);

  sponge_test_ae(s);
  sponge_test_hash(s);
  sponge_test_pointwise(s);

  return UNITY_END();
}
