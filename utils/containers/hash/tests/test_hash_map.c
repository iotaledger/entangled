/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/errors.h"
#include "utils/containers/hash/hash_int64_t_map.h"
#include "utils/containers/hash/tests/defs.h"

void test_hash_int64_t_map() {
  hash_to_int64_t_map_t map = NULL;
  hash_to_int64_t_map_entry_t* e = NULL;
  TEST_ASSERT(hash_to_int64_t_map_find(map, hash243_1, &e) == false);
  TEST_ASSERT(hash_to_int64_t_map_add(&map, hash243_1, 42) == RC_OK);
  TEST_ASSERT(hash_to_int64_t_map_find(map, hash243_1, &e));
  hash_to_int64_t_map_free(&map);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_hash_int64_t_map);

  return UNITY_END();
}
