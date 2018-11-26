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

#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/tests/common.h"
#include "mam/v2/trits.h"
#include "utils/macros.h"

void prng_test_do(prng_t *const prng) {
  flex_trit_t key[MAM2_PRNG_KEY_FLEX_SIZE];
  // TODO Remove when sponge handles flex_trit_t
  MAM2_TRITS_DEF(K, MAM2_PRNG_KEY_SIZE);
  TRIT_ARRAY_DECLARE(N, 18);
  MAM2_TRITS_DEF(Y1, 243 * 2 + 18);
  TRIT_ARRAY_DECLARE(Y1_arr, 243 * 2 + 18);
  MAM2_TRITS_DEF(Y2, 243 * 2 + 18);
  TRIT_ARRAY_DECLARE(Y2_arr, 243 * 2 + 18);

  // init K
  trits_set_zero(K);
  tryte_t const *const key_trytes =
      "NOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCD"
      "EFGHIJKLM";
  trytes_to_trits(key_trytes, K.p, MIN(strlen(key_trytes), K.n / RADIX));
  flex_trits_from_trytes(key, MAM2_PRNG_KEY_SIZE, key_trytes, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);

  sponge_init(prng->sponge);
  sponge_absorb(prng->sponge, MAM2_SPONGE_CTL_KEY, K);
  sponge_squeeze(prng->sponge, MAM2_SPONGE_CTL_KEY, K);
  // init N
  trit_array_set_null(&N);

  prng_init(prng, prng->sponge, key);
  prng_gen(prng, 0, &N, &Y1_arr);
  prng_gen(prng, 1, &N, &Y2_arr);

  flex_trits_to_trits(Y1.p + Y1.d, 243 * 2 + 18, Y1_arr.trits, 243 * 2 + 18,
                      243 * 2 + 18);
  flex_trits_to_trits(Y2.p + Y2.d, 243 * 2 + 18, Y2_arr.trits, 243 * 2 + 18,
                      243 * 2 + 18);

  int d = trits_cmp_grlex(Y1, Y2);
  TEST_ASSERT(d != 0);
}

void prng_test() {
  test_sponge_t _sponge;
  test_prng_t _prng;

  sponge_t *sponge = test_sponge_init(&_sponge);
  prng_t *prng = test_prng_init(&_prng, sponge);
  prng_test_do(prng);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(prng_test);

  return UNITY_END();
}
