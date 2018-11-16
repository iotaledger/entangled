
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/buffers.h"
#include "mam/v2/curl.h"
#include "mam/v2/mam.h"
#include "mam/v2/mss.h"
#include "mam/v2/ntru.h"
#include "mam/v2/pb3.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/tests/common.h"
#include "mam/v2/trits.h"
#include "mam/v2/wots.h"
#include "utils/macros.h"

#include <string.h>
#include <unity/unity.h>

#include <memory.h>
#include <stdio.h>

static void sponge_test_hash(isponge *s) {
  bool_t r = 1;
  MAM2_TRITS_DEF(X0, MAM2_SPONGE_RATE * 3);
  trits_t Xs[3], X;
  MAM2_TRITS_DEF(Y1, 243);
  MAM2_TRITS_DEF(Y2, 243);
  size_t n;

  for (n = 0; n < MAM2_SPONGE_RATE * 3; n += MAM2_SPONGE_RATE / 2) {
    X = trits_take(X0, n);
    trits_set_zero(X);
    sponge_hash(s, X, Y1);

    Xs[0] = trits_take(X, n / 3);
    Xs[1] = trits_take(trits_drop(X, n / 3), n / 4);
    Xs[2] = trits_drop(X, n / 3 + n / 4);
    sponge_hashn(s, 3, Xs, Y2);
    TEST_ASSERT(0 == trits_cmp_grlex(Y1, Y2));
  }
}

static bool_t sponge_test_ae(isponge *s) {
#define MAM2_SPONGE_TEST_MAX_K 1110
  size_t k, i;
  MAM2_TRITS_DEF(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF(X, MAM2_SPONGE_TEST_MAX_K);
  MAM2_TRITS_DEF(Y, MAM2_SPONGE_TEST_MAX_K);
  MAM2_TRITS_DEF(Z, MAM2_SPONGE_TEST_MAX_K);
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
  for (i = 0; i < MAM2_SPONGE_TEST_MAX_K / 3; ++i)
    trits_put3(trits_drop(X, 3 * i), (tryte_t)((i % 27) - 13));
  // init K
  trits_set_zero(K);
  trytes_to_trits(
      "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
      "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
      "NOPQRSTUVWXYZ9ABCDEFGHIJKLM",
      K.p, 81);

  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  sponge_squeeze(s, MAM2_SPONGE_CTL_KEY, K);

  for (i = 0; (k = ks[i++]) <= MAM2_SPONGE_TEST_MAX_K;) {
    X = MAM2_TRITS_INIT(X, k);
    Y = MAM2_TRITS_INIT(Y, k);
    Z = MAM2_TRITS_INIT(Z, k);
    sponge_init(s);
    sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    sponge_encr(s, X, Y);  // Y=E(X)
    sponge_init(s);
    sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    sponge_decr(s, Y, Z);  // Z=D(E(X))
    if (0 != trits_cmp_grlex(X, Z)) return 0;
    sponge_init(s);
    sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    sponge_encr(s, Z, Z);  // Z=E(Z=X)
    if (0 != trits_cmp_grlex(Y, Z)) return 0;
    sponge_init(s);
    sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    sponge_decr(s, Z, Z);  // Z=D(Z=E(X))
    if (0 != trits_cmp_grlex(X, Z)) return 0;
  }

#undef MAM2_SPONGE_TEST_MAX_K
  return 1;
}

static void sponge_test_pointwise(isponge *s) {
  bool_t r, ok;

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

void sponge_test(isponge *s) {
  sponge_test_ae(s);
  sponge_test_hash(s);
  sponge_test_pointwise(s);
}

int main(void) {
  UNITY_BEGIN();

  test_sponge_t _s[1];
  isponge *s = test_sponge_init(_s);

  sponge_test(s);

  return UNITY_END();
}
