/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>
#include <unity/unity.h>

#include "common/trinary/trit_tryte.h"

#define TRYTES_IN "AZN9AZN9AZN9AZN9AZN9"
#define EXP                                                                                                            \
  1, 0, 0, -1, 0, 0, -1, -1, -1, 0, 0, 0, 1, 0, 0, -1, 0, 0, -1, -1, -1, 0, 0, 0, 1, 0, 0, -1, 0, 0, -1, -1, -1, 0, 0, \
      0, 1, 0, 0, -1, 0, 0, -1, -1, -1, 0, 0, 0, 1, 0, 0, -1, 0, 0, -1, -1, -1, 0, 0, 0

void test_trits_to_trytes(void) {
  trit_t trits[] = {EXP};
  tryte_t trytes[20];
  tryte_t exp[] = {TRYTES_IN};
  trits_to_trytes(trits, trytes, 60);
  TEST_ASSERT_EQUAL_MEMORY(exp, trytes, 20);
}

void test_trytes_to_trits(void) {
  tryte_t trytes[] = {TRYTES_IN};
  trit_t trits[60];
  trit_t exp[] = {EXP};
  trytes_to_trits(trytes, trits, 20);
  TEST_ASSERT_EQUAL_MEMORY(exp, trits, 60);
}

void test_get_trit_at(void) {
  tryte_t trytes[] = {TRYTES_IN};
  trit_t trit;
  trit = get_trit_at(trytes, strlen(TRYTES_IN), 0);
  TEST_ASSERT_EQUAL(trit, 1);
  trit = get_trit_at(trytes, strlen(TRYTES_IN), 3);
  TEST_ASSERT_EQUAL(trit, -1);
  trit = get_trit_at(trytes, strlen(TRYTES_IN), 11);
  TEST_ASSERT_EQUAL(trit, 0);
}

void test_set_trit_at(void) {
  tryte_t trytes[] = {TRYTES_IN};
  trit_t trit;
  set_trit_at(trytes, strlen(TRYTES_IN), 0, -1);
  trit = get_trit_at(trytes, strlen(TRYTES_IN), 0);
  TEST_ASSERT_EQUAL(trit, -1);
  trit = get_trit_at(trytes, strlen(TRYTES_IN), 1);
  TEST_ASSERT_EQUAL(trit, 0);
  trit = get_trit_at(trytes, strlen(TRYTES_IN), 2);
  TEST_ASSERT_EQUAL(trit, 0);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trits_to_trytes);
  RUN_TEST(test_trytes_to_trits);
  RUN_TEST(test_get_trit_at);
  RUN_TEST(test_set_trit_at);

  return UNITY_END();
}
