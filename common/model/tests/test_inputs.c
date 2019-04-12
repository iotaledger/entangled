/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/model/inputs.h"
#include "common/model/tests/defs.h"

void test_empty_inputs() {
  inputs_t input_list = {};
  TEST_ASSERT(inputs_balance(&input_list) == 0);
  TEST_ASSERT(inputs_len(&input_list) == 0);
  inputs_clear(&input_list);

  input_t input = {};
  flex_trit_t empty[FLEX_TRIT_SIZE_243] = {};
  TEST_ASSERT_EQUAL_MEMORY(input.address, empty, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(input.balance == 0);
  TEST_ASSERT(input.key_index == 0);
  TEST_ASSERT(input.security == 0);
}

void test_inputs_operation() {
  inputs_t input_list = {};
  input_t input_a = {
      .balance = 100,
      .key_index = 0,
      .security = 2,
      .address = {},
  };

  input_t input_b = {
      .balance = 200,
      .key_index = 3,
      .security = 1,
      .address = {},
  };
  flex_trits_from_trytes(input_a.address, NUM_TRITS_ADDRESS, TEST_ADDRESS_0, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
  flex_trits_from_trytes(input_b.address, NUM_TRITS_ADDRESS, TEST_ADDRESS_1, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  TEST_ASSERT(inputs_append(&input_list, &input_a) == RC_OK);
  TEST_ASSERT(inputs_balance(&input_list) == 100);
  TEST_ASSERT(inputs_len(&input_list) == 1);

  input_t* tmp = inputs_at(&input_list, 0);
  TEST_ASSERT_EQUAL_MEMORY(tmp->address, input_a.address, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(tmp->balance == 100);
  TEST_ASSERT(tmp->key_index == 0);
  TEST_ASSERT(tmp->security == 2);

  TEST_ASSERT(inputs_append(&input_list, &input_b) == RC_OK);
  TEST_ASSERT(inputs_balance(&input_list) == 300);
  TEST_ASSERT(inputs_len(&input_list) == 2);

  tmp = inputs_at(&input_list, 1);
  TEST_ASSERT_EQUAL_MEMORY(tmp->address, input_b.address, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(tmp->balance == 200);
  TEST_ASSERT(tmp->key_index == 3);
  TEST_ASSERT(tmp->security == 1);

  // cleanup list
  inputs_clear(&input_list);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_empty_inputs);
  RUN_TEST(test_inputs_operation);

  return UNITY_END();
}
