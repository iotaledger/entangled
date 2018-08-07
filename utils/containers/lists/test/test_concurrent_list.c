/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/network/lists/concurrent_list.c.inc"
#include "common/network/lists/concurrent_list.h.inc"

DECLARE_CONCURRENT_LIST_OF(int)
DEFINE_CONCURRENT_LIST_OF(int)

static bool int_cmp(int const *const a, int const *const b) { return *a == *b; }

void test_list() {
  int data;
  concurrent_list_of_int *list = NULL;
  concurrent_list_of_int *nlist = NULL;

  TEST_ASSERT_EQUAL_INT(INIT_CONCURRENT_LIST_OF(int, list, int_cmp),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_NOT_NULL(list);

  TEST_ASSERT_NULL(list->vtable->front(NULL));
  TEST_ASSERT_NULL(list->vtable->front(list));
  TEST_ASSERT_NULL(list->vtable->back(NULL));
  TEST_ASSERT_NULL(list->vtable->back(list));
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(NULL), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(NULL), 0);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 0);

  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(NULL, 42),
                        CONCURRENT_LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 42),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 42);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 42);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 1);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_front(NULL, &data),
                        CONCURRENT_LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_front(list, NULL),
                        CONCURRENT_LIST_NULL_DATA);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_front(list, &data),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 42);
  TEST_ASSERT_NULL(list->vtable->front(list));
  TEST_ASSERT_NULL(list->vtable->back(list));
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 0);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_front(list, &data),
                        CONCURRENT_LIST_EMPTY);

  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(NULL, 42),
                        CONCURRENT_LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 42),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 42);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 42);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 1);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_back(NULL, &data),
                        CONCURRENT_LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_back(list, NULL),
                        CONCURRENT_LIST_NULL_DATA);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_back(list, &data),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(data, 42);
  TEST_ASSERT_NULL(list->vtable->back(list));
  TEST_ASSERT_NULL(list->vtable->front(list));
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 0);
  TEST_ASSERT_EQUAL_INT(list->vtable->pop_back(list, &data),
                        CONCURRENT_LIST_EMPTY);

  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 0),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 1),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 2),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 3),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 4),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 5),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 6),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 7),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_front(list, 8),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->push_back(list, 9),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 10);
  concurrent_list_node_of_int_t *tmp = list->front;
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

  TEST_ASSERT_EQUAL_INT(list->vtable->remove(NULL, 0),
                        CONCURRENT_LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 8), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 8), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 9);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 6);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 9);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 9), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 9), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 8);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 6);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 7);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 0), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 0), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 7);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 6);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 7);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 4), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 4), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 6);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 6);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 7);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 5), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 5), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 5);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 6);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 7);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 6), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 6), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 4);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 2);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 7);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 7), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 7), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 3);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 2);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 3);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 1), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 1), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 2);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 2);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 3);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 2), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 2), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 1);
  TEST_ASSERT_EQUAL_INT(*list->vtable->front(list), 3);
  TEST_ASSERT_EQUAL_INT(*list->vtable->back(list), 3);
  TEST_ASSERT_EQUAL_INT(list->vtable->remove(list, 3), CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_EQUAL_INT(list->vtable->contains(list, 3), false);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 0);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), true);
  TEST_ASSERT_NULL(list->vtable->front(list));
  TEST_ASSERT_NULL(list->vtable->back(list));

  TEST_ASSERT_EQUAL_INT(DESTROY_CONCURRENT_LIST_OF(int, nlist),
                        CONCURRENT_LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(DESTROY_CONCURRENT_LIST_OF(int, list),
                        CONCURRENT_LIST_SUCCESS);
  TEST_ASSERT_NULL(list);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_list);

  return UNITY_END();
}
