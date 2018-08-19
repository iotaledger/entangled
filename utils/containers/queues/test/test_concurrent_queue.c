/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "utils/containers/queues/concurrent_queue.c.inc"
#include "utils/containers/queues/concurrent_queue.h.inc"

DECLARE_CQ(int)
DEFINE_CQ(int)

typedef concurrent_queue_int conqueue_int;

void test_concurrent_queue() {
  int data;
  conqueue_int *queue = NULL;
  conqueue_int *nqueue = NULL;

  TEST_ASSERT_EQUAL_INT(CQ_INIT(int, queue), CQ_SUCCESS);
  TEST_ASSERT_NOT_NULL(queue);

  TEST_ASSERT_NULL(queue->vtable->front(NULL));
  TEST_ASSERT_NULL(CQ_FRONT(queue));
  TEST_ASSERT_NULL(queue->vtable->back(NULL));
  TEST_ASSERT_NULL(CQ_BACK(queue));
  TEST_ASSERT_EQUAL_INT(queue->vtable->empty(NULL), true);
  TEST_ASSERT_EQUAL_INT(CQ_EMPTY(queue), true);
  TEST_ASSERT_EQUAL_INT(queue->vtable->size(NULL), 0);
  TEST_ASSERT_EQUAL_INT(CQ_SIZE(queue), 0);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(NULL, 0), CQ_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(queue->vtable->pop(NULL, &data), CQ_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(CQ_POP(queue, NULL), CQ_NULL_DATA);

  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 0), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 1), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 2), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 3), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 4), CQ_SUCCESS);

  TEST_ASSERT_EQUAL_INT(*CQ_FRONT(queue), 0);
  TEST_ASSERT_EQUAL_INT(*CQ_BACK(queue), 4);
  TEST_ASSERT_EQUAL_INT(CQ_EMPTY(queue), false);
  TEST_ASSERT_EQUAL_INT(CQ_SIZE(queue), 5);

  TEST_ASSERT_EQUAL_INT(CQ_POP(queue, &data), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 0);

  TEST_ASSERT_EQUAL_INT(*CQ_FRONT(queue), 1);
  TEST_ASSERT_EQUAL_INT(*CQ_BACK(queue), 4);
  TEST_ASSERT_EQUAL_INT(CQ_EMPTY(queue), false);
  TEST_ASSERT_EQUAL_INT(CQ_SIZE(queue), 4);

  TEST_ASSERT_EQUAL_INT(CQ_POP(queue, &data), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 1);

  TEST_ASSERT_EQUAL_INT(*CQ_FRONT(queue), 2);
  TEST_ASSERT_EQUAL_INT(*CQ_BACK(queue), 4);
  TEST_ASSERT_EQUAL_INT(CQ_EMPTY(queue), false);
  TEST_ASSERT_EQUAL_INT(CQ_SIZE(queue), 3);

  TEST_ASSERT_EQUAL_INT(CQ_POP(queue, &data), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 2);

  TEST_ASSERT_EQUAL_INT(*CQ_FRONT(queue), 3);
  TEST_ASSERT_EQUAL_INT(*CQ_BACK(queue), 4);
  TEST_ASSERT_EQUAL_INT(CQ_EMPTY(queue), false);
  TEST_ASSERT_EQUAL_INT(CQ_SIZE(queue), 2);

  TEST_ASSERT_EQUAL_INT(CQ_POP(queue, &data), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 3);

  TEST_ASSERT_EQUAL_INT(*CQ_FRONT(queue), 4);
  TEST_ASSERT_EQUAL_INT(*CQ_BACK(queue), 4);
  TEST_ASSERT_EQUAL_INT(CQ_EMPTY(queue), false);
  TEST_ASSERT_EQUAL_INT(CQ_SIZE(queue), 1);

  TEST_ASSERT_EQUAL_INT(CQ_POP(queue, &data), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 4);

  TEST_ASSERT_NULL(CQ_FRONT(queue));
  TEST_ASSERT_NULL(CQ_BACK(queue));
  TEST_ASSERT_EQUAL_INT(CQ_EMPTY(queue), true);
  TEST_ASSERT_EQUAL_INT(CQ_SIZE(queue), 0);

  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 0), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 1), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 2), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 3), CQ_SUCCESS);
  TEST_ASSERT_EQUAL_INT(CQ_PUSH(queue, 4), CQ_SUCCESS);

  TEST_ASSERT_EQUAL_INT(CQ_DESTROY(int, nqueue), CQ_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(CQ_DESTROY(int, queue), CQ_SUCCESS);
  TEST_ASSERT_NULL(queue);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_concurrent_queue);

  return UNITY_END();
}
