/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/crypto/curl-p/hashcash.h"
#include "common/crypto/curl-p/trit.h"

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

void run_pd_test(CurlType type, unsigned short mwm) {
  Curl curl;
  curl.type = type;
  trit_t trits[] = {TRYTES_IN};
  trit_t hash[HASH_LENGTH_TRIT];
  curl_init(&curl);
  curl_absorb(&curl, trits, HASH_LENGTH_TRIT);
  PearlDiverStatus result = hashcash(&curl, 0, HASH_LENGTH_TRIT, mwm);
  curl_squeeze(&curl, hash, HASH_LENGTH_TRIT);

  TEST_ASSERT_EQUAL_INT8(PEARL_DIVER_SUCCESS, result);
  TEST_ASSERT_EQUAL_INT8_ARRAY(zeros, &(curl.state[HASH_LENGTH_TRIT - mwm]), mwm * sizeof(trit_t));
  curl_reset(&curl);
}

void run_pd_test_fail(CurlType type, unsigned short mwm) {
  Curl curl;
  curl.type = type;
  trit_t trits[] = {TRYTES_IN};
  curl_init(&curl);
  curl_absorb(&curl, trits, HASH_LENGTH_TRIT);
  // [0,mwm/2) is too short range to find mwm zero trits in hash, so the search should fail
  PearlDiverStatus result = hashcash(&curl, 0, mwm / 2, mwm);

  TEST_ASSERT_EQUAL_INT8(PEARL_DIVER_ERROR, result);
  curl_reset(&curl);
}

void test_pd_27_works(void) { run_pd_test(CURL_P_27, 13); }

void test_pd_81_works(void) { run_pd_test(CURL_P_81, 10); }

void test_pd_27_fail_works(void) { run_pd_test_fail(CURL_P_27, 13); }

void test_pd_81_fail_works(void) { run_pd_test_fail(CURL_P_81, 10); }

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_pd_27_works);
  RUN_TEST(test_pd_81_works);
  RUN_TEST(test_pd_27_fail_works);
  RUN_TEST(test_pd_81_fail_works);

  return UNITY_END();
}
