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

void test_list() {
  int data;
  list_of_int *list = NULL;
  list_of_int *nlist = NULL;

  TEST_ASSERT_EQUAL_INT(INIT_LIST_OF(int, list), LIST_SUCCESS);
  TEST_ASSERT_NOT_NULL(list);

  TEST_ASSERT_NULL(list->vtable->front(NULL));
  TEST_ASSERT_NULL(list->vtable->front(list));
  TEST_ASSERT_NULL(list->vtable->back(NULL));
  TEST_ASSERT_NULL(list->vtable->back(list));
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(NULL), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->empty(list), true);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(NULL), 0);
  TEST_ASSERT_EQUAL_INT(list->vtable->size(list), 0);

  TEST_ASSERT_EQUAL_INT(DESTROY_LIST_OF(int, nlist), LIST_NULL_SELF);
  TEST_ASSERT_EQUAL_INT(DESTROY_LIST_OF(int, list), LIST_SUCCESS);
  TEST_ASSERT_NULL(list);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_list);

  return UNITY_END();
}
