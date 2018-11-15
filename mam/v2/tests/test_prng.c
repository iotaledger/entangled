
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

void prng_test_do(prng_t *p) {
  bool_t r = 1;
  MAM2_TRITS_DEF(K, MAM2_PRNG_KEY_SIZE);
  MAM2_TRITS_DEF(N, 18);
  MAM2_TRITS_DEF(Y1, 243 * 2 + 18);
  MAM2_TRITS_DEF(Y2, 243 * 2 + 18);

  // init K
  trits_set_zero(K);
  const char *k_str =
      "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
      "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
      "NOPQRSTUVWXYZ9ABCDEFGHIJKLM";
  trytes_to_trits(k_str, K.p, MIN(strlen(k_str), K.n / RADIX));

  sponge_init(p->sponge);
  sponge_absorb(p->sponge, MAM2_SPONGE_CTL_KEY, K);
  sponge_squeeze(p->sponge, MAM2_SPONGE_CTL_KEY, K);
  // init N
  trits_set_zero(N);

  prng_init(p, p->sponge, K);
  prng_gen(p, 0, N, Y1);
  prng_gen(p, 1, N, Y2);

  int d = trits_cmp_grlex(Y1, Y2);
  TEST_ASSERT(d != 0);
}

void prng_test() {
  test_sponge_t _s[1];
  test_prng_t _p[1];

  isponge *s = test_sponge_init(_s);
  prng_t *p = test_prng_init(_p, s);
  prng_test_do(p);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(prng_test);

  return UNITY_END();
}
