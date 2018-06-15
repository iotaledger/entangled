#include <unity/unity.h>

#include "common/sign/v1/iss_curl.h"
#include "common/trinary/trit_tryte.h"
#include "mam/merkle.h"

static size_t const expected_merkle_size = 20;
static size_t const expected_tree_depth = 5;
static size_t const expected_siblings[] = {11, 13, 2, 16};

void test_merkle(void) {
  char *const seed =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQR"
      "STUVWXYZ9";
  trit_t seed_trits[HASH_LENGTH];
  trytes_to_trits((tryte_t *)seed, seed_trits, HASH_LENGTH / 3);
  size_t start = 1;
  size_t index = 5;
  size_t leaf_count = 9;
  size_t security = 1;
  size_t size = merkle_size(leaf_count);
  trit_t merkle_tree[size * HASH_LENGTH];
  trit_t siblings[(merkle_depth(size) - 1) * HASH_LENGTH];
  Curl c;
  c.type = CURL_P_27;
  init_curl(&c);

  TEST_ASSERT_EQUAL_INT(expected_merkle_size, size);
  TEST_ASSERT_EQUAL_INT(expected_tree_depth, merkle_depth(size));
  TEST_ASSERT_EQUAL_INT(0, merkle_create(merkle_tree, seed_trits, start,
                                         leaf_count, security, &c));
  TEST_ASSERT_EQUAL_INT(0,
                        merkle_branch(siblings, merkle_tree, size * HASH_LENGTH,
                                      merkle_depth(size), index));
  for (size_t i = 0; i < merkle_depth(size) - 1; i++) {
    TEST_ASSERT_EQUAL_MEMORY(&merkle_tree[expected_siblings[i] * HASH_LENGTH],
                             &siblings[i * HASH_LENGTH], HASH_LENGTH);
  }
  trit_t hash[HASH_LENGTH];
  memcpy(hash, &merkle_tree[12 * HASH_LENGTH], HASH_LENGTH);
  curl_reset(&c);
  TEST_ASSERT_EQUAL_INT(
      0, merkle_root(hash, siblings, merkle_depth(size) - 1, index, &c));
  TEST_ASSERT_EQUAL_MEMORY(merkle_tree, hash, HASH_LENGTH);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_merkle);

  return UNITY_END();
}
