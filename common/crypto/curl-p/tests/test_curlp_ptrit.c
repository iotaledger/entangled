/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/crypto/curl-p/ptrit.h"
#include "common/crypto/curl-p/tests/test_curlp_ptrit.h"

void run_curl_p_test(PCurl *curl, ptrit_t *exp, CurlType curl_type) {
  ptrit_t trits[] = {TRYTES_IN};
  ptrit_t hash[HASH_LENGTH_TRIT];

  ptrit_curl_init(curl, curl_type);
  ptrit_curl_absorb(curl, trits, 6);
  ptrit_curl_squeeze(curl, hash, HASH_LENGTH_TRIT);
  ptrit_curl_reset(curl);
  TEST_ASSERT_EQUAL_MEMORY(exp, hash, sizeof(hash));
}

void test_curl_p_27_works(void) {
  PCurl curl;
  curl.type = CURL_P_27;
  ptrit_t trits_exp[] = {EXP_27};
  run_curl_p_test(&curl, trits_exp, CURL_P_27);
}

void test_curl_p_81_works(void) {
  PCurl curl;
  curl.type = CURL_P_81;
  ptrit_t trits_exp[] = {EXP_81};
  run_curl_p_test(&curl, trits_exp, CURL_P_81);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_curl_p_27_works);
  RUN_TEST(test_curl_p_81_works);

  return UNITY_END();
}
