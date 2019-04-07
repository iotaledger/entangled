/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/crypto/iss/v2/iss_curl.h"
#include "common/trinary/trit_tryte.h"
#include "utils/merkle.h"

static size_t const expected_merkle_size = 63;
static size_t const expected_tree_depth = 6;

void test_merkle(void) {
  char *const seed =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQR"
      "STUVWXYZ9";
  trit_t seed_trits[HASH_LENGTH_TRIT];
  trytes_to_trits((tryte_t *)seed, seed_trits, HASH_LENGTH_TRYTE);
  size_t start = 7;
  size_t leaf_count = 32;
  size_t security = 1;
  size_t size = merkle_size(leaf_count);
  trit_t merkle_tree[size * HASH_LENGTH_TRIT];
  trit_t depth = merkle_depth(size);
  trit_t siblings[(depth - 1) * HASH_LENGTH_TRIT];
  trit_t hash[HASH_LENGTH_TRIT];
  Curl c;
  c.type = CURL_P_27;
  curl_init(&c);

  TEST_ASSERT_EQUAL_INT(expected_merkle_size, size);
  TEST_ASSERT_EQUAL_INT(expected_tree_depth, depth);
  TEST_ASSERT_EQUAL_INT(0, merkle_create(merkle_tree, leaf_count, seed_trits, start, security, &c));
  for (size_t i = 0; i < leaf_count; i++) {
    TEST_ASSERT_EQUAL_INT(0, merkle_branch(merkle_tree, siblings, size * HASH_LENGTH_TRIT, depth, i, leaf_count));
    memcpy(hash,
           &merkle_tree[merkle_node_index(depth - 1, merkle_leaf_index(i, leaf_count), depth - 1) * HASH_LENGTH_TRIT],
           HASH_LENGTH_TRIT);
    curl_reset(&c);
    merkle_root(hash, siblings, depth - 1, i, &c);
    TEST_ASSERT_EQUAL_MEMORY(merkle_tree, hash, HASH_LENGTH_TRIT);
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_merkle);

  return UNITY_END();
}
