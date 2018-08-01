#include <unity/unity.h>

#include "common/curl-p/hashcash.h"
#include "common/curl-p/trit.h"

#define TRYTES_IN                                                              \
  -1, 1, -1, -1, 1, -1, 1, 1, 0, -1, 0, 0, 1, 0, 1, 0, 0, 0, -1, -1, -1, -1,   \
      0, 0, -1, 0, 0, 1, 0, 0, -1, 0, 0, 1, -1, -1, 1, -1, 1, -1, -1, 1, 0, 1, \
      0, 0, 0, 1, -1, 0, -1, 1, -1, -1, 0, 0, 0, -1, 0, 0, 1, -1, -1, 0, 0, 0, \
      -1, 0, 0, 0, -1, -1, 0, 1, 1, -1, 1, 1, 1, 1, -1, 0, -1, 0, -1, 0, -1,   \
      0, -1, -1, -1, -1, 0, 1, -1, 0, -1, -1, 0, 0, 0, 0, 0, 1, 1, 0, 1, -1,   \
      0, -1, -1, -1, 0, 0, 1, 0, -1, -1, -1, -1, 0, -1, -1, -1, 0, -1, 0, 0,   \
      -1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 0, -1, 1, -1, -1, -1, 0, 1, 1, 0,  \
      -1, 0, 1, 0, 0, 1, 1, 0, 0, -1, -1, 1, 0, 0, 0, 0, -1, 1, 0, 1, 0, 0, 0, \
      1, -1, 1, -1, 0, 0, -1, 1, 1, -1, 0, 0, 1, -1, 0, 1, 0, -1, 1, -1, 0, 0, \
      1, -1, -1, -1, 0, 1, 0, -1, -1, 0, 1, 0, 0, 0, 1, -1, 1, -1, 0, 1, -1,   \
      -1, 0, 0, 0, -1, -1, 1, 1, 0, 1, -1, 0, 0, 0, -1, 0, -1, 0, -1, -1, -1,  \
      -1, 0, 1, -1, -1, 0, 1
const trit_t zeros[HASH_LENGTH] = {0};

void run_pd_test(CurlType type, unsigned short mwm) {
  Curl curl;
  curl.type = type;
  trit_t trits[] = {TRYTES_IN};
  trit_t hash[HASH_LENGTH];
  init_curl(&curl);
  curl_absorb(&curl, trits, HASH_LENGTH);
  PearlDiverStatus result = hashcash(&curl, BODY, 0, HASH_LENGTH, mwm);
  curl_squeeze(&curl, hash, HASH_LENGTH);

  TEST_ASSERT_EQUAL_INT8(PEARL_DIVER_SUCCESS, result);
  TEST_ASSERT_EQUAL_INT8_ARRAY(zeros, &(curl.state[HASH_LENGTH - mwm]),
                               mwm * sizeof(trit_t));
  curl_reset(&curl);
}

void test_pd_27_works(void) { run_pd_test(CURL_P_27, 13); }

void test_pd_81_works(void) { run_pd_test(CURL_P_81, 10); }

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_pd_27_works);
  RUN_TEST(test_pd_81_works);

  return UNITY_END();
}
