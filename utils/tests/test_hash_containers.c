/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/tests/test_hash_containers.h"

void test_hash_queue() {
  hash_queue_t queue = NULL;
  hash_queue_push(&queue, hash243_1);
  hash_queue_push(&queue, hash243_2);

  TEST_ASSERT_EQUAL_INT(2, hash_queue_count(&queue));
  TEST_ASSERT_FALSE(hash_queue_empty(queue));

  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash_queue_peek(queue),
                           FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash_queue_at(&queue, 0),
                           FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_2, hash_queue_at(&queue, 1),
                           FLEX_TRIT_SIZE_243);
  TEST_ASSERT_NULL(hash_queue_at(&queue, 2));

  hash_queue_pop(&queue);
  TEST_ASSERT_EQUAL_INT(1, hash_queue_count(&queue));
  hash_queue_pop(&queue);
  TEST_ASSERT_EQUAL_INT(0, hash_queue_count(&queue));

  TEST_ASSERT_TRUE(hash_queue_empty(queue));
  TEST_ASSERT_NULL(hash_queue_peek(queue));
  hash_queue_free(&queue);
}

void test_hash_stack() {
  hash_stack_t stack = NULL;

  hash_stack_push(&stack, hash243_1);
  hash_stack_push(&stack, hash243_2);

  TEST_ASSERT_EQUAL_INT(2, hash_stack_count(stack));
  TEST_ASSERT_FALSE(hash_stack_empty(stack));

  TEST_ASSERT_EQUAL_MEMORY(hash243_2, hash_stack_peek(stack),
                           FLEX_TRIT_SIZE_243);

  hash_stack_pop(&stack);
  TEST_ASSERT_EQUAL_INT(1, hash_stack_count(stack));
  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash_stack_peek(stack),
                           FLEX_TRIT_SIZE_243);

  hash_stack_pop(&stack);
  TEST_ASSERT_EQUAL_INT(0, hash_stack_count(stack));
  TEST_ASSERT_NULL(hash_stack_peek(stack));

  hash_stack_free(&stack);
}
int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_hash_queue);
  RUN_TEST(test_hash_stack);

  return UNITY_END();
}
