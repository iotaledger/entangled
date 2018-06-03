#include <stdio.h>
#include <unity/unity.h>

#include "common/trinary/trit_array.h"
#include "common/trinary/trit_byte.h"

#if !defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE) && \
    !defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
#define TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE
#endif

#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
#define TRITS_IN  -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1, 1, 0, -1, 1
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)  
#define TRITS_IN 0x53, 0x14, 0x1f, 0xC5, 0x01
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
#define TRITS_IN  0x23, 0x98, 0x25, 0x02
#endif
#define NUM_TRITS 17

void test_trit_array_static(void) {
  TRIT_ARRAY_ASSIGN(test, NUM_TRITS, TRITS_IN);
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&test, 0));
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&test, 1));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&test, 2));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&test, 10));
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&test, 11));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&test, 12));
}

void test_trit_array_static_set(void) {
  TRIT_ARRAY_ASSIGN(test, NUM_TRITS, TRITS_IN);
  trit_array_set_at(&test, 10, 0);
  trit_array_set_at(&test, 11, 1);
  trit_array_set_at(&test, 12, -1);
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&test, 10));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&test, 11));
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&test, 12));
}

void test_trit_array_static_slice(void) {
  TRIT_ARRAY_ASSIGN(from, NUM_TRITS, TRITS_IN);
  TRIT_ARRAY_DECLARE(to, 7);
  trit_array_p pto = &to;
  trit_array_slice(&from, &pto, 6, 7);
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&to, 0));
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&to, 1));
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&to, 2));
  TEST_ASSERT_EQUAL_INT(-1, trit_array_at(&to, 3));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&to, 4));
  TEST_ASSERT_EQUAL_INT(0, trit_array_at(&to, 5));
  TEST_ASSERT_EQUAL_INT(1, trit_array_at(&to, 6));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trit_array_static);
  RUN_TEST(test_trit_array_static_set);
  RUN_TEST(test_trit_array_static_slice);
  
  return UNITY_END();
}
