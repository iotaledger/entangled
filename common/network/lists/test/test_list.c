/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/network/lists/list.c.inc"
#include "common/network/lists/list.h.inc"

DECLARE_LIST_OF(int)
DEFINE_LIST_OF(int)

static bool int_cmp(int a, int b) { return a == b; }

void test_list() {
  int data;
  list_of_int *list = NULL;
  list_of_int *nlist = NULL;

  TEST_ASSERT_EQUAL_INT(INIT_LIST_OF(int, list, int_cmp), LIST_SUCCESS);
  TEST_ASSERT_NOT_NULL(list);

  TEST_ASSERT_NULL(list->vtable->front(NULL));
  TEST_ASSERT_NULL(list->vtable->front(list));
  TEST_ASSERT_NULL(list->vtable->back(NULL));
  TEST_ASSERT_NULL(list->vtable->back(list));
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(NULL), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(NULL), 0);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 0);

  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(NULL, 42), LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 42), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 42);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 42);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 1);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_front(NULL, &data), LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_front(list, NULL), LIST_NULL_DATA);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_front(list, &data), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 42);
  TEST_ASSERT_NULL(list->vtable->front(list));
  TEST_ASSERT_NULL(list->vtable->back(list));
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 0);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_front(list, &data), LIST_EMPTY);

  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(NULL, 42), LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 42), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 42);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 42);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 1);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_back(NULL, &data), LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_back(list, NULL), LIST_NULL_DATA);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_back(list, &data), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 42);
  TEST_ASSERT_NULL(list->vtable->back(list));
  TEST_ASSERT_NULL(list->vtable->front(list));
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 0);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_back(list, &data), LIST_EMPTY);

  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 0), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 1), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 2), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 3), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 4), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 5), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 6), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 7), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 8), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 9), LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 10);
  list_node_of_int_t *tmp = list->front;
  TEST_ASSERT_NOT_NULL(tmp);
  TEST_ASSERT_EQUAL_INT(tmp->data, 8);
  tmp = tmp->next;
  TEST_ASSERT_EQUAL_INT(tmp->data, 6);
  tmp = tmp->next;
  TEST_ASSERT_EQUAL_INT(tmp->data, 4);
  tmp = tmp->next;
  TEST_ASSERT_EQUAL_INT(tmp->data, 2);
  tmp = tmp->next;
  TEST_ASSERT_EQUAL_INT(tmp->data, 0);
  tmp = tmp->next;
  TEST_ASSERT_EQUAL_INT(tmp->data, 1);
  tmp = tmp->next;
  TEST_ASSERT_EQUAL_INT(tmp->data, 3);
  tmp = tmp->next;
  TEST_ASSERT_EQUAL_INT(tmp->data, 5);
  tmp = tmp->next;
  TEST_ASSERT_EQUAL_INT(tmp->data, 7);
  tmp = tmp->next;
  TEST_ASSERT_EQUAL_INT(tmp->data, 9);
  tmp = tmp->next;
  TEST_ASSERT_NULL(tmp);

  TEST_ASSERT_EQUAL_INT(list->vtable->contains(NULL, 0), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 0), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 1), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 2), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 3), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 4), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 5), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 6), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 7), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 8), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 9), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 10), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 42), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, -1), false);

  TEST_ASSERT_EQUAL_INT(DESTROY_LIST_OF(int, nlist), LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(DESTROY_LIST_OF(int, list), LIST_SUCCESS);
  TEST_ASSERT_NULL(list);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_list);

  return UNITY_END();
}
