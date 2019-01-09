/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "utils/containers/lock_free/lf_mpmc_queue_int.h"

void queue_integers_same_thread() {
  int i = 0;
  bool has_dequeued = false;
  lf_mpmc_queue_int_t queue;

  TEST_ASSERT(lf_mpmc_queue_int_init(&queue, sizeof(int)) == RC_OK);

  TEST_ASSERT_TRUE(LF_MPMC_QUEUE_IS_EMPTY(&queue));
  TEST_ASSERT_EQUAL_INT(lf_mpmc_queue_int_count(&queue), 0);

  TEST_ASSERT(lf_mpmc_queue_int_enqueue(&queue, &i) == RC_OK);
  TEST_ASSERT(lf_mpmc_queue_int_trydequeue(&queue, &i, &has_dequeued) == RC_OK);
  TEST_ASSERT_TRUE(has_dequeued);

  for (int i = 0; i < 1000; ++i) {
    TEST_ASSERT(lf_mpmc_queue_int_enqueue(&queue, &i) == RC_OK);
  }

  TEST_ASSERT_FALSE(LF_MPMC_QUEUE_IS_EMPTY(&queue));
  TEST_ASSERT_EQUAL_INT(lf_mpmc_queue_int_count(&queue), 1000);

  int dequeued = 0;
  for (int i = 0; i < 1000; ++i) {
    TEST_ASSERT(lf_mpmc_queue_int_dequeue(&queue, &dequeued) == RC_OK);
    TEST_ASSERT_EQUAL_INT(i, dequeued);
  }

  TEST_ASSERT(lf_mpmc_queue_int_trydequeue(&queue, &i, &has_dequeued) == RC_OK);
  TEST_ASSERT_FALSE(has_dequeued);

  TEST_ASSERT_TRUE(LF_MPMC_QUEUE_IS_EMPTY(&queue));
  TEST_ASSERT_EQUAL_INT(lf_mpmc_queue_int_count(&queue), 0);

  TEST_ASSERT(lf_mpmc_queue_int_destroy(&queue) == RC_OK);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(queue_integers_same_thread);

  return UNITY_END();
}
