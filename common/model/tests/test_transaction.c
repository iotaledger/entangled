/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>
#include <unity/unity.h>

#include "common/model/transaction.h"

static const char* TRYTES[] = {};

void test_serialize(void) {
}

void test_deserialize(void) {
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize);
  RUN_TEST(test_deserialize);

  return UNITY_END();
}
