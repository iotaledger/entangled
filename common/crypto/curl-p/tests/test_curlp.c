/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/crypto/curl-p/trit.h"

#define OTHER_TRITS_IN -1, 1, -1, -1, 1, -1
#define TRITS_IN                                                                                                       \
  -1, 1, -1, -1, 1, -1, 1, 1, 0, -1, 0, 0, 1, 0, 1, 0, 0, 0, -1, -1, -1, -1, 0, 0, -1, 0, 0, 1, 0, 0, -1, 0, 0, 1, -1, \
      -1, 1, -1, 1, -1, -1, 1, 0, 1, 0, 0, 0, 1, -1, 0, -1, 1, -1, -1, 0, 0, 0, -1, 0, 0, 1, -1, -1, 0, 0, 0, -1, 0,   \
      0, 0, -1, -1, 0, 1, 1, -1, 1, 1, 1, 1, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1, -1, -1, 0, 1, -1, 0, -1, -1, 0, 0, 0, \
      0, 0, 1, 1, 0, 1, -1, 0, -1, -1, -1, 0, 0, 1, 0, -1, -1, -1, -1, 0, -1, -1, -1, 0, -1, 0, 0, -1, 1, 1, -1, -1,   \
      1, 1, -1, 1, -1, 1, 0, -1, 1, -1, -1, -1, 0, 1, 1, 0, -1, 0, 1, 0, 0, 1, 1, 0, 0, -1, -1, 1, 0, 0, 0, 0, -1, 1,  \
      0, 1, 0, 0, 0, 1, -1, 1, -1, 0, 0, -1, 1, 1, -1, 0, 0, 1, -1, 0, 1, 0, -1, 1, -1, 0, 0, 1, -1, -1, -1, 0, 1, 0,  \
      -1, -1, 0, 1, 0, 0, 0, 1, -1, 1, -1, 0, 1, -1, -1, 0, 0, 0, -1, -1, 1, 1, 0, 1, -1, 0, 0, 0, -1, 0, -1, 0, -1,   \
      -1, -1, -1, 0, 1, -1, -1, 0, 1
#define CURL_P_27_EXP                                                                                                  \
  -1, -1, -1, -1, 0, 0, 1, 1, -1, 1, 1, 0, -1, 1, 0, 1, 0, 0, 1, 0, -1, 1, 1, -1, -1, -1, 0, 1, 0, 1, -1, -1, 1, -1,   \
      -1, -1, -1, 1, 1, 1, 1, -1, 1, 1, 1, -1, 0, 1, -1, 1, 0, 0, 1, -1, 1, -1, 1, 0, 1, 0, 0, 1, -1, 1, 1, -1, 0, 0,  \
      1, 1, -1, 0, 1, 0, -1, 0, 0, 1, -1, -1, -1, 0, 0, -1, 1, 0, 0, -1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, -1, 1, 0,    \
      -1, 1, 0, 1, 1, 0, 0, -1, 1, -1, 1, 0, -1, 0, 1, 0, 1, -1, 1, -1, 0, 1, 0, 1, 1, 1, -1, 0, 1, -1, 0, 0, 0, 1, 0, \
      -1, 0, -1, 0, -1, -1, 1, -1, 1, 1, 0, -1, 1, 0, -1, 1, 0, 1, -1, 0, 0, 0, -1, 0, 0, -1, 0, -1, -1, 0, 0, -1, -1, \
      1, 1, -1, -1, -1, 0, -1, 0, -1, -1, 1, -1, -1, -1, -1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, -1, 1, 0, 1, -1, -1, -1,    \
      -1, 1, 0, 0, -1, 1, 1, 1, -1, 1, 0, -1, 0, 1, -1, 1, 1, 1, 0, 1, 1, 0, -1, 0, 1, 1, -1, 0, -1, 0, 1, 0, 0, 1, 1, \
      1, -1, 0, 1, -1, 0
#define CURL_P_81_EXP                                                                                                  \
  1, 1, 1, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, -1, 1, -1, 1, -1, -1, -1, 1, 1, 0, 0, 1, 0, 0, 1, 0, -1, 1, 0, 1, -1, 0, 1, \
      0, -1, 0, -1, 1, -1, -1, -1, -1, 0, 1, 0, 1, -1, 1, 0, 0, 0, 1, -1, 1, -1, -1, -1, 1, 0, 1, 1, 0, -1, 0, 0, 0,   \
      0, -1, 0, -1, 1, 1, 0, -1, 0, 1, 0, -1, 0, 1, -1, 1, -1, 0, -1, 0, -1, 0, 0, -1, 1, -1, 1, 0, 1, 1, 1, 1, 0, 0,  \
      -1, 1, 1, 0, 0, 1, 0, -1, -1, 1, -1, 0, 1, 1, 0, -1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, -1, 1, -1, -1, -1, 1, 1,   \
      0, 0, -1, -1, 1, -1, -1, -1, -1, -1, -1, 0, 0, 0, -1, 0, 1, 1, -1, -1, 1, 0, 1, 1, 1, 0, 1, 0, -1, 1, 0, -1, 0,  \
      1, 1, 1, 0, 1, 0, -1, 1, 1, -1, -1, -1, -1, 0, 1, -1, 0, 1, -1, 1, 1, 0, -1, 0, 1, -1, 0, 1, 0, 0, 1, 0, 1, -1,  \
      1, -1, 0, -1, 1, 0, -1, 0, -1, 0, -1, 0, 1, 0, 1, 0, -1, -1, 1, -1, 0, 1, 0, -1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1,   \
      -1, 1, 1, -1, 1

void run_curl_p_test(Curl *curl, trit_t *exp) {
  trit_t trits[] = {TRITS_IN};
  trit_t hash[HASH_LENGTH_TRIT];

  curl_init(curl);
  curl_absorb(curl, trits, HASH_LENGTH_TRIT);
  curl_squeeze(curl, hash, HASH_LENGTH_TRIT);
  curl_reset(curl);

  TEST_ASSERT_EQUAL_INT8_ARRAY(exp, hash, HASH_LENGTH_TRIT);
}

void test_curl_p_27_works(void) {
  Curl curl;
  curl.type = CURL_P_27;
  trit_t trits_exp[] = {CURL_P_27_EXP};
  run_curl_p_test(&curl, trits_exp);
  // Reset should work
  run_curl_p_test(&curl, trits_exp);
}

void test_curl_p_81_works(void) {
  Curl curl;
  curl.type = CURL_P_81;
  trit_t trits_exp[] = {CURL_P_81_EXP};
  run_curl_p_test(&curl, trits_exp);
  // Reset should work
  run_curl_p_test(&curl, trits_exp);
}

/*
void test_other(void)
{
        Curl curl;
        curl.type = CURL_P_27;
        trit_t hash[HASH_LENGTH_TRIT];

        trit_t trits[] = {OTHER_TRITS_IN};
        curl_init(&curl);
        curl_absorb(&curl, trits, HASH_LENGTH_TRIT);
        curl_squeeze(&curl, hash, HASH_LENGTH_TRIT);
        curl_reset(&curl);
        for(int i = 0; i < HASH_LENGTH_TRIT; i++) {
                fprintf(stderr, "%hhd, ", hash[i]);
        }
        fprintf(stderr, "\n");
        curl.type = CURL_P_81;
        curl_absorb(&curl, trits, HASH_LENGTH_TRIT);
        curl_squeeze(&curl, hash, HASH_LENGTH_TRIT);
        curl_reset(&curl);
        for(int i = 0; i < HASH_LENGTH_TRIT; i++) {
                fprintf(stderr, "%hhd, ", hash[i]);
        }
}
*/

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_curl_p_27_works);
  RUN_TEST(test_curl_p_81_works);
  // RUN_TEST(test_other);

  return UNITY_END();
}
