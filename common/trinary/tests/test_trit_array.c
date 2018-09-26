/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <unity/unity.h>

#include "common/trinary/trit_array.h"
#include "common/trinary/trit_byte.h"

#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
#define TRITS_IN -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1, 1, 0, -1, 1, 0
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
#define TRITS_IN 0x48, 0x4a, 0x53, 0x42, 0x44, 0x42
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
#define TRITS_IN 0x53, 0x14, 0x1f, 0xC5, 0x01
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
#define TRITS_IN 0x23, 0x98, 0x25, 0x02
#endif
#define TRYTES "HJSBDB"
#define TRITS_OUT -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1, 1, 0, -1, 1, 0
#define NUM_TRITS 18

void test_trit_array_static(void) {
  TRIT_ARRAY_MAKE(test, NUM_TRITS, TRITS_IN);
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&test, 0));
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&test, 1));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&test, 2));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&test, 10));
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&test, 11));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&test, 12));
}

void test_trit_array_static_set(void) {
  TRIT_ARRAY_MAKE(test, NUM_TRITS, TRITS_IN);
  trit_array_set_at(&test, 10, 0);
  trit_array_set_at(&test, 11, 1);
  trit_array_set_at(&test, 12, -1);
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&test, 10));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&test, 11));
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&test, 12));
}

void test_trit_array_static_slice(void) {
  TRIT_ARRAY_MAKE(from, NUM_TRITS, TRITS_IN);
  TRIT_ARRAY_DECLARE(to, 5);
  trit_array_slice(&from, &to, 8, 5);
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&to, 0));
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&to, 1));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&to, 2));
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&to, 3));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&to, 4));
}

void test_trit_array_static_insert(void) {
  TRIT_ARRAY_MAKE(from, NUM_TRITS, TRITS_IN);
  TRIT_ARRAY_DECLARE(to, 5);
  trit_array_slice(&from, &to, 8, 5);
  trit_array_insert(&from, &to, 4, 5);
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&from, 4));
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&from, 5));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&from, 6));
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&from, 7));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&from, 8));
}

void test_trit_array_static_to_int8(void) {
  int8_t trits[NUM_TRITS];
  trit_t trits_out[] = {TRITS_OUT};
  TRIT_ARRAY_MAKE(test, NUM_TRITS, TRITS_IN);
  trit_array_to_int8(&test, trits, NUM_TRITS);
  TEST_ASSERT_EQUAL_MEMORY(trits_out, trits, NUM_TRITS);
}

void test_trytes_to_trit_array(void) {
  trit_t trits_out[] = {TRITS_IN};
  trit_array_p out_array = trit_array_new_from_trytes((tryte_t*)TRYTES);
  TEST_ASSERT_EQUAL_MEMORY(trits_out, out_array->trits, out_array->num_bytes);
  trit_array_free(out_array);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trit_array_static);
  RUN_TEST(test_trit_array_static_set);
  RUN_TEST(test_trit_array_static_slice);
  RUN_TEST(test_trit_array_static_insert);
  RUN_TEST(test_trit_array_static_to_int8);
  RUN_TEST(test_trytes_to_trit_array);

  return UNITY_END();
}
