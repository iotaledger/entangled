/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/crypto/curl-p/hamming.h"
#include "common/crypto/curl-p/trit.h"

#define START 9

#define TRYTES_IN                                                                                                      \
  -1, 1, -1, -1, 1, -1, 1, 1, 0, -1, 0, 0, 1, 0, 1, 0, 0, 0, -1, -1, -1, -1, 0, 0, -1, 0, 0, 1, 0, 0, -1, 0, 0, 1, -1, \
      -1, 1, -1, 1, -1, -1, 1, 0, 1, 0, 0, 0, 1, -1, 0, -1, 1, -1, -1, 0, 0, 0, -1, 0, 0, 1, -1, -1, 0, 0, 0, -1, 0,   \
      0, 0, -1, -1, 0, 1, 1, -1, 1, 1, 1, 1, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1, -1, -1, 0, 1, -1, 0, -1, -1, 0, 0, 0, \
      0, 0, 1, 1, 0, 1, -1, 0, -1, -1, -1, 0, 0, 1, 0, -1, -1, -1, -1, 0, -1, -1, -1, 0, -1, 0, 0, -1, 1, 1, -1, -1,   \
      1, 1, -1, 1, -1, 1, 0, -1, 1, -1, -1, -1, 0, 1, 1, 0, -1, 0, 1, 0, 0, 1, 1, 0, 0, -1, -1, 1, 0, 0, 0, 0, -1, 1,  \
      0, 1, 0, 0, 0, 1, -1, 1, -1, 0, 0, -1, 1, 1, -1, 0, 0, 1, -1, 0, 1, 0, -1, 1, -1, 0, 0, 1, -1, -1, -1, 0, 1, 0,  \
      -1, -1, 0, 1, 0, 0, 0, 1, -1, 1, -1, 0, 1, -1, -1, 0, 0, 0, -1, -1, 1, 1, 0, 1, -1, 0, 0, 0, -1, 0, -1, 0, -1,   \
      -1, -1, -1, 0, 1, -1, -1, 0, 1
const trit_t zeros[HASH_LENGTH_TRIT] = {0};

void run_pd_test(Curl *curl, size_t security) {
  unsigned short i, j;
  short sum;
  trit_t trits[] = {TRYTES_IN};
  trit_t hash[HASH_LENGTH_TRIT];
  curl_init(curl);
  curl_absorb(curl, trits, HASH_LENGTH_TRIT);
  PearlDiverStatus result = hamming(curl, 0, HASH_LENGTH_TRIT, security);
  TEST_ASSERT_EQUAL_INT(PEARL_DIVER_SUCCESS, result);
  curl_squeeze(curl, hash, HASH_LENGTH_TRIT);

  for (sum = 0, i = 0; i < security; i++) {
    for (j = i * HASH_LENGTH_TRYTE; j < (i + 1) * HASH_LENGTH_TRYTE; j++) {
      sum += curl->state[j];
    }
    if (i < security - 1) {
      TEST_ASSERT_NOT_EQUAL(0, sum);
    }
  }
  TEST_ASSERT_EQUAL_INT16(0, sum);
}

void test_pd_27_works(void) {
  unsigned short i;
  Curl curl;
  curl.type = CURL_P_27;
  for (i = 1; i <= 3; i++) {
    run_pd_test(&curl, i);
  }
  curl_reset(&curl);
}

void test_pd_81_works(void) {
  unsigned short i;
  Curl curl;
  curl.type = CURL_P_81;
  for (i = 1; i <= 3; i++) {
    run_pd_test(&curl, i);
  }
  curl_reset(&curl);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_pd_27_works);
  RUN_TEST(test_pd_81_works);

  return UNITY_END();
}
