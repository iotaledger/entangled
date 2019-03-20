/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "common/crypto/iss/v2/iss_curl.h"
#include "common/trinary/trit_tryte.h"
#include "mam/prototype/mam.h"
#include "utils/merkle.h"

void test_mam_sec(int security) {
  char *const seed =
      "TX9XRR9SRCOBMTYDTMKNEIJCSZIMEUPWCNLC9DPDZKKAEMEFVSTEVUFTRUZXEHLULEIYJIEO"
      "WIC9STAHW";
  char *const message =
      "D9999999JDLILILID9999999D9999999GC999999FB999999EA999999D9999999YELILILI"
      "GGOFQGHCMCOC9999WAFEA999UA999999JHTB9999VFLILILIFGOFQGHCCCOC9999ABFEA999"
      "UA999999WGSB9999SGLILILIEGOFQGHCTBOC99999BFEA999UA999999WGSB9999PHLILILI"
      "DGOFQGHCJBOC9999ZAFEA999VA999999WGSB9999N999X999D999H999L999P999T999F999"
      "H999D999";
  char *const side_key =
      "QOLOACG9BNUYLERQTZPPW9VKIOPDRTPMFZCYWGNVKIZJEYBWJDXASOXNDMZGBNYFVBCFBQBX"
      "SCCAFFRIO";

  trit_t seed_trits[3 * strlen(seed) * sizeof(trit_t)];
  trit_t side_key_trits[3 * strlen(side_key) * sizeof(trit_t)];
  trit_t message_trits[3 * strlen(message) * sizeof(trit_t)];

  trytes_to_trits((tryte_t *)seed, seed_trits, strlen(seed));
  trytes_to_trits((tryte_t *)side_key, side_key_trits, strlen(side_key));
  trytes_to_trits((tryte_t *)message, message_trits, strlen(message));

  size_t index = 7;
  size_t message_length = strlen(message) * 3;

  size_t start = 0;
  size_t count = 8;
  size_t next_start = start + count;
  size_t next_count = 1;
  size_t tree_size = merkle_size(count);
  size_t next_tree_size = merkle_size(next_count);
  trit_t merkle_tree[tree_size * HASH_LENGTH_TRIT];
  trit_t next_root[next_tree_size * HASH_LENGTH_TRIT];
  int payload_length = payload_min_length(message_length, tree_size * HASH_LENGTH_TRIT, index, security);
  trit_t *payload = (trit_t *)malloc(payload_length * sizeof(trit_t));

  Curl curl;
  curl.type = CURL_P_27;
  curl_init(&curl);
  TEST_ASSERT_EQUAL_INT(0, merkle_create(merkle_tree, count, seed_trits, start, security, &curl));
  curl_reset(&curl);
  TEST_ASSERT_EQUAL_INT(0, merkle_create(next_root, next_count, seed_trits, next_start, security, &curl));
  curl_reset(&curl);
  payload_length = mam_create(payload, payload_length, message_trits, message_length, side_key_trits,
                              strlen(side_key) * 3, merkle_tree, tree_size * HASH_LENGTH_TRIT, count, index, next_root,
                              start, seed_trits, security, &curl);
  TEST_ASSERT_TRUE(payload_length != -1);
  curl_reset(&curl);

  size_t parsed_index = -1;
  size_t parsed_message_length = -1;
  trit_t parsed_next_root[HASH_LENGTH_TRIT];
  size_t parsed_message_trits[message_length];
  size_t parsed_security = -1;

  TEST_ASSERT_EQUAL_INT(
      0, mam_parse(payload, payload_length, (trit_t *)parsed_message_trits, &parsed_message_length, side_key_trits,
                   strlen(side_key) * 3, merkle_tree, &parsed_index, parsed_next_root, &parsed_security, &curl));

  TEST_ASSERT_EQUAL_INT(index, parsed_index);
  TEST_ASSERT_EQUAL_MEMORY(next_root, parsed_next_root, HASH_LENGTH_TRIT);
  TEST_ASSERT_EQUAL_MEMORY(message_trits, parsed_message_trits, parsed_message_length);
  TEST_ASSERT_EQUAL_INT(security, parsed_security);
  free(payload);
}

void test_mam() {
  test_mam_sec(1);
  test_mam_sec(2);
  test_mam_sec(3);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_mam);

  return UNITY_END();
}
