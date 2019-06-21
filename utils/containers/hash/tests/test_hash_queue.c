/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "utils/containers/hash/hash243_queue.h"
#include "utils/containers/hash/tests/defs.h"

void test_hash243_queue() {
  hash243_queue_t queue = NULL;
  hash243_queue_entry_t *entry = NULL;

  hash243_queue_push(&queue, hash243_1);
  hash243_queue_push(&queue, hash243_2);

  TEST_ASSERT_EQUAL_INT(2, hash243_queue_count(queue));
  TEST_ASSERT_FALSE(hash243_queue_empty(queue));

  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash243_queue_peek(queue), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash243_queue_at(queue, 0), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_2, hash243_queue_at(queue, 1), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_NULL(hash243_queue_at(queue, 2));

  entry = hash243_queue_pop(&queue);
  TEST_ASSERT_EQUAL_MEMORY(hash243_1, entry->hash, FLEX_TRIT_SIZE_243);
  free(entry);
  TEST_ASSERT_EQUAL_INT(1, hash243_queue_count(queue));
  entry = hash243_queue_pop(&queue);
  TEST_ASSERT_EQUAL_MEMORY(hash243_2, entry->hash, FLEX_TRIT_SIZE_243);
  free(entry);
  TEST_ASSERT_EQUAL_INT(0, hash243_queue_count(queue));

  TEST_ASSERT_TRUE(hash243_queue_empty(queue));
  TEST_ASSERT_NULL(hash243_queue_peek(queue));

  hash243_queue_free(&queue);
}

void test_hash243_copy() {
  hash243_queue_t src = NULL;
  hash243_queue_t dest = NULL;

  hash243_queue_push(&src, hash243_1);
  hash243_queue_push(&src, hash243_2);
  hash243_queue_push(&src, hash243_1);

  // copy n elements from src to dest.
  TEST_ASSERT(hash243_queue_copy(&dest, src, 2) == RC_OK);
  TEST_ASSERT_EQUAL_INT(2, hash243_queue_count(dest));
  TEST_ASSERT_EQUAL_MEMORY(hash243_queue_at(dest, 0), hash243_queue_at(src, 0), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_queue_at(dest, 1), hash243_queue_at(src, 1), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_NULL(hash243_queue_at(dest, 2));

  hash243_queue_free(&src);
  hash243_queue_free(&dest);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_hash243_queue);
  RUN_TEST(test_hash243_copy);

  return UNITY_END();
}
