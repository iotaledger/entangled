/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "utils/containers/queues/concurrent_queue.c.inc"
#include "utils/containers/queues/concurrent_queue.h.inc"

DECLARE_CONCURRENT_QUEUE_OF(int)
DEFINE_CONCURRENT_QUEUE_OF(int)

typedef concurrent_queue_of_int conqueue_int;

void test_concurrent_queue() {
  int data;
  conqueue_int *queue = NULL;
  conqueue_int *nqueue = NULL;

  TEST_ASSERT_EQUAL_INT(INIT_CONCURRENT_QUEUE_OF(int, queue),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_NOT_NULL(queue);

  TEST_ASSERT_NULL(queue->vtable->front(NULL));
  TEST_ASSERT_NULL(queue->vtable->front(queue));
  TEST_ASSERT_NULL(queue->vtable->back(NULL));
  TEST_ASSERT_NULL(queue->vtable->back(queue));
  TEST_ASSERT_EQUAL_INT(queue->vtable->empty(NULL), true);
  TEST_ASSERT_EQUAL_INT(queue->vtable->empty(queue), true);
  TEST_ASSERT_EQUAL_INT(queue->vtable->size(NULL), 0);
  TEST_ASSERT_EQUAL_INT(queue->vtable->size(queue), 0);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(NULL, 0),
                        CONCURRENT_QUEUE_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(queue->vtable->pop(NULL, &data),
                        CONCURRENT_QUEUE_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(queue->vtable->pop(queue, NULL),
                        CONCURRENT_QUEUE_NULL_DATA);

  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 0),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 1),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 2),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 3),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 4),
                        CONCURRENT_QUEUE_SUCCESS);

  TEST_ASSERT_EQUAL_INT(*queue->vtable->front(queue), 0);
  TEST_ASSERT_EQUAL_INT(*queue->vtable->back(queue), 4);
  TEST_ASSERT_EQUAL_INT(queue->vtable->empty(queue), false);
  TEST_ASSERT_EQUAL_INT(queue->vtable->size(queue), 5);

  TEST_ASSERT_EQUAL_INT(queue->vtable->pop(queue, &data),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 0);

  TEST_ASSERT_EQUAL_INT(*queue->vtable->front(queue), 1);
  TEST_ASSERT_EQUAL_INT(*queue->vtable->back(queue), 4);
  TEST_ASSERT_EQUAL_INT(queue->vtable->empty(queue), false);
  TEST_ASSERT_EQUAL_INT(queue->vtable->size(queue), 4);

  TEST_ASSERT_EQUAL_INT(queue->vtable->pop(queue, &data),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 1);

  TEST_ASSERT_EQUAL_INT(*queue->vtable->front(queue), 2);
  TEST_ASSERT_EQUAL_INT(*queue->vtable->back(queue), 4);
  TEST_ASSERT_EQUAL_INT(queue->vtable->empty(queue), false);
  TEST_ASSERT_EQUAL_INT(queue->vtable->size(queue), 3);

  TEST_ASSERT_EQUAL_INT(queue->vtable->pop(queue, &data),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 2);

  TEST_ASSERT_EQUAL_INT(*queue->vtable->front(queue), 3);
  TEST_ASSERT_EQUAL_INT(*queue->vtable->back(queue), 4);
  TEST_ASSERT_EQUAL_INT(queue->vtable->empty(queue), false);
  TEST_ASSERT_EQUAL_INT(queue->vtable->size(queue), 2);

  TEST_ASSERT_EQUAL_INT(queue->vtable->pop(queue, &data),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 3);

  TEST_ASSERT_EQUAL_INT(*queue->vtable->front(queue), 4);
  TEST_ASSERT_EQUAL_INT(*queue->vtable->back(queue), 4);
  TEST_ASSERT_EQUAL_INT(queue->vtable->empty(queue), false);
  TEST_ASSERT_EQUAL_INT(queue->vtable->size(queue), 1);

  TEST_ASSERT_EQUAL_INT(queue->vtable->pop(queue, &data),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 4);

  TEST_ASSERT_NULL(queue->vtable->front(queue));
  TEST_ASSERT_NULL(queue->vtable->back(queue));
  TEST_ASSERT_EQUAL_INT(queue->vtable->empty(queue), true);
  TEST_ASSERT_EQUAL_INT(queue->vtable->size(queue), 0);

  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 0),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 1),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 2),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 3),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_EQUAL_INT(queue->vtable->push(queue, 4),
                        CONCURRENT_QUEUE_SUCCESS);

  TEST_ASSERT_EQUAL_INT(DESTROY_CONCURRENT_QUEUE_OF(int, nqueue),
                        CONCURRENT_QUEUE_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(DESTROY_CONCURRENT_QUEUE_OF(int, queue),
                        CONCURRENT_QUEUE_SUCCESS);
  TEST_ASSERT_NULL(queue);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_concurrent_queue);

  return UNITY_END();
}
