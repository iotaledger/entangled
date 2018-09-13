/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/model/bundle.h"

void test_normalized_bundle(void) {}

void test_flex_normalized_bundle(void) {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_normalized_bundle);
  RUN_TEST(test_flex_normalized_bundle);

  return UNITY_END();
}
