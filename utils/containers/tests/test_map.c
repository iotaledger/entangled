/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdbool.h>
#include <unity/unity.h>
#include "common/errors.h"
#include "utils/containers/hash/tests/defs.h"
#include "utils/containers/person_example.h"
#include "utils/containers/person_example_t_to_int_map.h"

void person_to_int_map() {
  person_example_t p1;
  memset(p1.name, 0, 128);
  memcpy(p1.hash, hash243_1, FLEX_TRIT_SIZE_243);
  memcpy(p1.name, "Satoshi", strlen("Satoshi"));
  int p1_value = 999999999;

  person_example_t p2;
  memset(p1.name, 0, 128);
  memcpy(p1.hash, hash243_2, FLEX_TRIT_SIZE_243);
  memcpy(p1.name, "Satoshi", strlen("Satoshi"));
  int p2_value = 3;  // Fake satoshi is poor

  person_example_t_to_int_map_t map;
  person_example_t_to_int_map_entry_t* e = NULL;

  TEST_ASSERT(person_example_t_to_int_map_init(&map, sizeof(person_example_t), sizeof(int)) == RC_OK);

  TEST_ASSERT(person_example_t_to_int_map_find(map, &p1, &e) == false);
  TEST_ASSERT(person_example_t_to_int_map_add(&map, &p1, &p1_value) == RC_OK);
  TEST_ASSERT(person_example_t_to_int_map_find(map, &p1, &e) == true);
  TEST_ASSERT_EQUAL_INT(p1_value, *e->value);
  TEST_ASSERT(person_example_t_to_int_map_add(&map, &p2, &p2_value) == RC_OK);
  TEST_ASSERT(person_example_t_to_int_map_find(map, &p2, &e) == true);
  TEST_ASSERT_EQUAL_INT(p2_value, *e->value);
  person_example_t_to_int_map_free(&map);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(person_to_int_map);

  return UNITY_END();
}
