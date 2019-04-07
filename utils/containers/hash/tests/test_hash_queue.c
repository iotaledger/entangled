/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/hash/hash243_queue.h"
#include "utils/containers/hash/tests/defs.h"

void test_hash243_queue() {
  hash243_queue_t queue = NULL;

  hash243_queue_push(&queue, hash243_1);
  hash243_queue_push(&queue, hash243_2);

  TEST_ASSERT_EQUAL_INT(2, hash243_queue_count(queue));
  TEST_ASSERT_FALSE(hash243_queue_empty(queue));

  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash243_queue_peek(queue), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash243_queue_at(&queue, 0), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_2, hash243_queue_at(&queue, 1), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_NULL(hash243_queue_at(&queue, 2));

  hash243_queue_pop(&queue);
  TEST_ASSERT_EQUAL_INT(1, hash243_queue_count(queue));
  hash243_queue_pop(&queue);
  TEST_ASSERT_EQUAL_INT(0, hash243_queue_count(queue));

  TEST_ASSERT_TRUE(hash243_queue_empty(queue));
  TEST_ASSERT_NULL(hash243_queue_peek(queue));

  hash243_queue_free(&queue);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_hash243_queue);

  return UNITY_END();
}
