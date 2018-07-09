/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/network/queues/concurrent_queue.c.inc"
#include "common/network/queues/concurrent_queue.h.inc"

DECLARE_CONCURRENT_QUEUE_OF(int)
DEFINE_CONCURRENT_QUEUE_OF(int)

typedef concurrent_queue_of_int conqueue_int;

void test_concurrent_queue() {
  int data;
  conqueue_int *c = NEW_CONCURRENT_QUEUE_OF_int();
  TEST_ASSERT_NOT_NULL(c);

  TEST_ASSERT_NULL(c->vtable->front(NULL));
  TEST_ASSERT_NULL(c->vtable->front(c));
  TEST_ASSERT_NULL(c->vtable->back(NULL));
  TEST_ASSERT_NULL(c->vtable->back(c));
  TEST_ASSERT_EQUAL_INT(c->vtable->empty(NULL), 1);
  TEST_ASSERT_EQUAL_INT(c->vtable->empty(c), 1);
  TEST_ASSERT_EQUAL_INT(c->vtable->size(NULL), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->size(c), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->push(NULL, 0), 1);
  TEST_ASSERT_EQUAL_INT(c->vtable->pop(NULL, &data), 1);
  TEST_ASSERT_EQUAL_INT(c->vtable->pop(c, NULL), 1);

  TEST_ASSERT_EQUAL_INT(c->vtable->push(c, 0), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->push(c, 1), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->push(c, 2), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->push(c, 3), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->push(c, 4), 0);

  TEST_ASSERT_EQUAL_INT(*c->vtable->front(c), 0);
  TEST_ASSERT_EQUAL_INT(*c->vtable->back(c), 4);
  TEST_ASSERT_EQUAL_INT(c->vtable->empty(c), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->size(c), 5);

  TEST_ASSERT_EQUAL_INT(c->vtable->pop(c, &data), 0);

  TEST_ASSERT_EQUAL_INT(data, 0);
  TEST_ASSERT_EQUAL_INT(*c->vtable->front(c), 1);
  TEST_ASSERT_EQUAL_INT(*c->vtable->back(c), 4);
  TEST_ASSERT_EQUAL_INT(c->vtable->empty(c), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->size(c), 4);

  TEST_ASSERT_EQUAL_INT(c->vtable->pop(c, &data), 0);

  TEST_ASSERT_EQUAL_INT(data, 1);
  TEST_ASSERT_EQUAL_INT(*c->vtable->front(c), 2);
  TEST_ASSERT_EQUAL_INT(*c->vtable->back(c), 4);
  TEST_ASSERT_EQUAL_INT(c->vtable->empty(c), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->size(c), 3);

  TEST_ASSERT_EQUAL_INT(c->vtable->pop(c, &data), 0);

  TEST_ASSERT_EQUAL_INT(data, 2);
  TEST_ASSERT_EQUAL_INT(*c->vtable->front(c), 3);
  TEST_ASSERT_EQUAL_INT(*c->vtable->back(c), 4);
  TEST_ASSERT_EQUAL_INT(c->vtable->empty(c), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->size(c), 2);

  TEST_ASSERT_EQUAL_INT(c->vtable->pop(c, &data), 0);

  TEST_ASSERT_EQUAL_INT(data, 3);
  TEST_ASSERT_EQUAL_INT(*c->vtable->front(c), 4);
  TEST_ASSERT_EQUAL_INT(*c->vtable->back(c), 4);
  TEST_ASSERT_EQUAL_INT(c->vtable->empty(c), 0);
  TEST_ASSERT_EQUAL_INT(c->vtable->size(c), 1);

  TEST_ASSERT_EQUAL_INT(c->vtable->pop(c, &data), 0);

  TEST_ASSERT_EQUAL_INT(data, 4);
  TEST_ASSERT_NULL(c->vtable->front(c));
  TEST_ASSERT_NULL(c->vtable->back(c));
  TEST_ASSERT_EQUAL_INT(c->vtable->empty(c), 1);
  TEST_ASSERT_EQUAL_INT(c->vtable->size(c), 0);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_concurrent_queue);

  return UNITY_END();
}
