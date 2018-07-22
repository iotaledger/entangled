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

void test_list() {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_list);

  return UNITY_END();
}
