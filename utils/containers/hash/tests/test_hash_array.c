/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/hash/hash_array.h"
#include "utils/containers/hash/tests/defs.h"

void test_empty_hash243_array() {
  hash243_array_p hashes = hash243_array_new();
  TEST_ASSERT_EQUAL_INT(0, hash_array_len(hashes));

  TEST_ASSERT_NULL(hash_array_at(hashes, 0));
  TEST_ASSERT_NULL(hash_array_at(hashes, 1));

  hash_array_free(hashes);
}

void test_hash243_array() {
  hash243_array_p hashes = hash243_array_new();
  hash_array_push(hashes, hash243_1);
  hash_array_push(hashes, hash243_2);
  TEST_ASSERT_EQUAL_INT(2, hash_array_len(hashes));

  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash_array_at(hashes, 0), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_2, hash_array_at(hashes, 1), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_NULL(hash_array_at(hashes, 2));
  TEST_ASSERT_NULL(hash_array_at(hashes, -1));

  hash_array_free(hashes);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_empty_hash243_array);
  RUN_TEST(test_hash243_array);

  return UNITY_END();
}
