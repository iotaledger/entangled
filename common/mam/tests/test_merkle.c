#include <unity/unity.h>

#include "common/sign/v2/iss_curl.h"
#include "common/trinary/trit_tryte.h"
#include "mam/merkle.h"

static size_t const expected_merkle_size = 20;
static size_t const expected_tree_depth = 5;
static trit_t const expected_merkle_root[] = {
    1,  -1, -1, -1, -1, 0,  0,  -1, 1,  0,  -1, 1,  0,  -1, 1,  1,  -1, 0,  -1,
    -1, -1, -1, -1, 1,  1,  0,  1,  0,  0,  1,  -1, -1, 1,  1,  0,  1,  0,  -1,
    1,  0,  0,  -1, 1,  1,  -1, -1, -1, -1, 0,  0,  -1, -1, 0,  -1, 0,  1,  1,
    0,  -1, 0,  0,  -1, -1, 1,  0,  -1, 1,  1,  -1, 1,  1,  0,  1,  -1, 1,  -1,
    0,  1,  -1, 0,  -1, 1,  -1, -1, -1, 0,  1,  -1, -1, -1, 1,  -1, 1,  -1, 1,
    0,  0,  -1, 1,  1,  -1, -1, 0,  0,  0,  0,  0,  1,  -1, 1,  0,  -1, 0,  -1,
    0,  -1, 0,  -1, 0,  -1, 1,  0,  1,  -1, 1,  -1, 1,  0,  1,  0,  -1, 1,  0,
    0,  -1, -1, 1,  1,  1,  1,  0,  1,  1,  -1, 1,  -1, 0,  0,  0,  -1, -1, 1,
    0,  1,  0,  -1, 1,  -1, -1, 1,  0,  1,  0,  -1, 0,  -1, -1, 1,  1,  1,  1,
    1,  0,  1,  1,  -1, 1,  0,  -1, 1,  1,  0,  1,  -1, 1,  0,  -1, -1, 1,  0,
    1,  -1, 1,  0,  1,  1,  0,  -1, -1, 0,  1,  1,  1,  0,  -1, -1, -1, 1,  1,
    0,  -1, 1,  0,  1,  -1, 0,  0,  -1, -1, 0,  -1, -1, 1,  1,  -1, 0,  -1, 0,
    -1, 1,  -1, 0,  0,  -1, 0,  1,  0,  -1, 0,  1,  -1, 0,  1};

void test_merkle(void) {
  char *const seed =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQR"
      "STUVWXYZ9";
  trit_t seed_trits[HASH_LENGTH];
  trytes_to_trits((tryte_t *)seed, seed_trits, HASH_LENGTH / 3);
  size_t start = 1;
  // size_t index = 5;
  size_t leaf_count = 9;
  size_t security = 1;
  size_t key_length = security * ISS_KEY_LENGTH;
  size_t size = merkle_size(leaf_count);
  trit_t merkle_tree[size * HASH_LENGTH];
  Curl c;
  c.type = CURL_P_27;
  init_curl(&c);

  TEST_ASSERT_EQUAL_INT(expected_merkle_size, size);
  TEST_ASSERT_EQUAL_INT(expected_tree_depth, merkle_depth(size));
  TEST_ASSERT_EQUAL_INT(0, merkle_create(merkle_tree, seed_trits, start,
                                         leaf_count, key_length, &c));
  TEST_ASSERT_EQUAL_MEMORY(expected_merkle_root, merkle_tree, HASH_LENGTH);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_merkle);

  return UNITY_END();
}
