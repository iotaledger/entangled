/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/node/tips_cache.h"

void test_tips_cache() {
  tips_cache_t cache;
  flex_trit_t hashes[10][243];
  tryte_t trytes[81] =
      "A99999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999";

  for (size_t i = 0; i < 10; i++) {
    flex_trits_from_trytes(hashes[i], HASH_LENGTH_TRIT, trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    trytes[0]++;
  }

  TEST_ASSERT(tips_cache_init(&cache, 5) == RC_OK);

  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 0);

  TEST_ASSERT(tips_cache_add(&cache, hashes[0]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 1);

  TEST_ASSERT(tips_cache_add(&cache, hashes[1]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 2);

  TEST_ASSERT(tips_cache_add(&cache, hashes[2]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 3);

  TEST_ASSERT(tips_cache_add(&cache, hashes[3]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 4);

  TEST_ASSERT(tips_cache_add(&cache, hashes[4]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 5);

  TEST_ASSERT(tips_cache_add(&cache, hashes[5]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 5);

  TEST_ASSERT(tips_cache_add(&cache, hashes[6]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 5);

  TEST_ASSERT(tips_cache_add(&cache, hashes[7]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 5);

  TEST_ASSERT(tips_cache_remove(&cache, hashes[5]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 4);

  TEST_ASSERT(tips_cache_remove(&cache, hashes[5]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 4);

  TEST_ASSERT(tips_cache_remove(&cache, hashes[9]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 4);

  TEST_ASSERT(tips_cache_remove(&cache, hashes[6]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 3);

  TEST_ASSERT(tips_cache_add(&cache, hashes[1]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 4);

  TEST_ASSERT(tips_cache_add(&cache, hashes[1]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 4);

  TEST_ASSERT(tips_cache_add(&cache, hashes[2]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 5);

  TEST_ASSERT(tips_cache_add(&cache, hashes[8]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 5);

  {
    hash243_set_entry_t *iter = cache.tips;

    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[4], FLEX_TRIT_SIZE_243);
    iter = iter->hh.next;
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[7], FLEX_TRIT_SIZE_243);
    iter = iter->hh.next;
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[1], FLEX_TRIT_SIZE_243);
    iter = iter->hh.next;
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[2], FLEX_TRIT_SIZE_243);
    iter = iter->hh.next;
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[8], FLEX_TRIT_SIZE_243);

    TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 5);
    TEST_ASSERT_EQUAL_INT(tips_cache_solid_size(&cache), 0);
    TEST_ASSERT_EQUAL_INT(tips_cache_size(&cache), 5);

    TEST_ASSERT(tips_cache_set_solid(&cache, hashes[7]) == RC_OK);
    TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 4);
    TEST_ASSERT_EQUAL_INT(tips_cache_solid_size(&cache), 1);
    TEST_ASSERT_EQUAL_INT(tips_cache_size(&cache), 5);

    TEST_ASSERT(tips_cache_set_solid(&cache, hashes[2]) == RC_OK);
    TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 3);
    TEST_ASSERT_EQUAL_INT(tips_cache_solid_size(&cache), 2);
    TEST_ASSERT_EQUAL_INT(tips_cache_size(&cache), 5);

    TEST_ASSERT(tips_cache_set_solid(&cache, hashes[4]) == RC_OK);
    TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 2);
    TEST_ASSERT_EQUAL_INT(tips_cache_solid_size(&cache), 3);
    TEST_ASSERT_EQUAL_INT(tips_cache_size(&cache), 5);

    TEST_ASSERT(tips_cache_set_solid(&cache, hashes[4]) == RC_OK);
    TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 2);
    TEST_ASSERT_EQUAL_INT(tips_cache_solid_size(&cache), 3);
    TEST_ASSERT_EQUAL_INT(tips_cache_size(&cache), 5);
  }

  {
    hash243_stack_t tips = NULL;
    hash243_stack_entry_t *iter = NULL;

    TEST_ASSERT(tips_cache_get_tips(&cache, &tips) == RC_OK);
    iter = tips;

    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[4], FLEX_TRIT_SIZE_243);
    iter = iter->next;
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[2], FLEX_TRIT_SIZE_243);
    iter = iter->next;
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[7], FLEX_TRIT_SIZE_243);
    iter = iter->next;
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[8], FLEX_TRIT_SIZE_243);
    iter = iter->next;
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hashes[1], FLEX_TRIT_SIZE_243);

    hash243_stack_free(&tips);
  }

  TEST_ASSERT(tips_cache_set_solid(&cache, hashes[1]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 1);
  TEST_ASSERT_EQUAL_INT(tips_cache_solid_size(&cache), 4);
  TEST_ASSERT_EQUAL_INT(tips_cache_size(&cache), 5);

  TEST_ASSERT(tips_cache_set_solid(&cache, hashes[8]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 0);
  TEST_ASSERT_EQUAL_INT(tips_cache_solid_size(&cache), 5);
  TEST_ASSERT_EQUAL_INT(tips_cache_size(&cache), 5);

  TEST_ASSERT(tips_cache_add(&cache, hashes[9]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 1);

  TEST_ASSERT(tips_cache_set_solid(&cache, hashes[9]) == RC_OK);
  TEST_ASSERT_EQUAL_INT(tips_cache_non_solid_size(&cache), 0);
  TEST_ASSERT_EQUAL_INT(tips_cache_solid_size(&cache), 5);
  TEST_ASSERT_EQUAL_INT(tips_cache_size(&cache), 5);

  TEST_ASSERT(tips_cache_destroy(&cache) == RC_OK);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_tips_cache);

  return UNITY_END();
}
