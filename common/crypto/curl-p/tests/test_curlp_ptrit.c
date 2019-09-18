/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/crypto/curl-p/ptrit.h"
#include "common/crypto/curl-p/tests/test_curlp_ptrit.h"
#include "common/crypto/curl-p/trit.h"

void test_pcurl(size_t round_count) {
  pcurl_t pcurl;
  Curl curl;
  curl.type = (CurlType)round_count;

  trit_t data[2][33];
  {
    trit_t const seed[] = {-1, 0, 1, 0, 1, 0, -1};
    curl_init(&curl);
    curl_absorb(&curl, seed, 7);
    curl_squeeze(&curl, data[0], 33);
    curl_squeeze(&curl, data[1], 33);
  }

  ptrit_t pdata[33];
  ptrits_set_slice(33, pdata, 0, data[0]);
  ptrits_set_slice(33, pdata, PTRIT_SIZE - 1, data[1]);
  trit_t hash[2][HASH_LENGTH_TRIT];
  ptrit_t phash[HASH_LENGTH_TRIT];

  curl_init(&curl);
  curl_absorb(&curl, data[0], 33);
  curl_squeeze(&curl, hash[0], HASH_LENGTH_TRIT);
  curl_init(&curl);
  curl_absorb(&curl, data[1], 33);
  curl_squeeze(&curl, hash[1], HASH_LENGTH_TRIT);

  pcurl_init(&pcurl, round_count);
  pcurl_hash_data(&pcurl, pdata, 33, phash);

  trit_t hashp[2][HASH_LENGTH_TRIT];
  ptrits_get_slice(HASH_LENGTH_TRIT, hashp[0], phash, 0);
  ptrits_get_slice(HASH_LENGTH_TRIT, hashp[1], phash, PTRIT_SIZE - 1);

  TEST_ASSERT_EQUAL_MEMORY(hash[0], hashp[0], sizeof(trit_t) * HASH_LENGTH_TRIT);
  TEST_ASSERT_EQUAL_MEMORY(hash[1], hashp[1], sizeof(trit_t) * HASH_LENGTH_TRIT);
}

void test_pcurl_27(void) { test_pcurl(27); }

void test_pcurl_81(void) { test_pcurl(81); }

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_pcurl_27);
  RUN_TEST(test_pcurl_81);

  return UNITY_END();
}
