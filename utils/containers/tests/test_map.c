/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>
#include "common/errors.h"
#include "utils/containers/double_to_double_map.h"

void double_to_double_map() {
  double key = 0.17;
  double_to_double_map_t map = NULL;
  double_to_double_map_entry_t* e = NULL;
  TEST_ASSERT(double_to_double_map_find(&map, &key, &e) == false);
  TEST_ASSERT(double_to_double_map_add(&map, &key, 42) == RC_OK);
  TEST_ASSERT(double_to_double_map_find(&map, &key, &e));
  double_to_double_map_free(&map);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(double_to_double_map);

  return UNITY_END();
}
