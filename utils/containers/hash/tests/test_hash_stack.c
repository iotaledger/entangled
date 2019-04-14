/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/hash/hash243_stack.h"
#include "utils/containers/hash/tests/defs.h"

void test_hash243_stack() {
  hash243_stack_t stack = NULL;

  TEST_ASSERT_TRUE(hash243_stack_empty(stack));

  TEST_ASSERT(hash243_stack_push(&stack, hash243_1) == RC_OK);
  TEST_ASSERT(hash243_stack_push(&stack, hash243_2) == RC_OK);

  TEST_ASSERT_FALSE(hash243_stack_empty(stack));

  TEST_ASSERT_EQUAL_INT(hash243_stack_count(stack), 2);

  TEST_ASSERT_EQUAL_MEMORY(hash243_2, hash243_stack_peek(stack), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_2, hash243_stack_at(stack, 0), FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash243_stack_at(stack, 1), FLEX_TRIT_SIZE_243);

  hash243_stack_pop(&stack);

  TEST_ASSERT_EQUAL_INT(hash243_stack_count(stack), 1);
  TEST_ASSERT_EQUAL_MEMORY(hash243_1, hash243_stack_peek(stack), FLEX_TRIT_SIZE_243);

  hash243_stack_pop(&stack);

  TEST_ASSERT_EQUAL_INT(hash243_stack_count(stack), 0);
  TEST_ASSERT_NULL(hash243_stack_peek(stack));
  TEST_ASSERT_TRUE(hash243_stack_empty(stack));

  hash243_stack_free(&stack);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_hash243_stack);

  return UNITY_END();
}
