#include <string.h>
#include <unity/unity.h>

#include "common/trinary/trit_tryte.h"

#define TRYTES_IN "AZN9"
#define EXP 1, 0, 0, -1, 0, 0, -1, -1, -1, 0, 0, 0

void test_tryte_to_trit(void) {
  trit_t trits[] = {EXP};
  tryte_t trytes[12];
  tryte_t exp[] = {TRYTES_IN};
  trits_to_trytes(trits, trytes, 12);
  TEST_ASSERT_EQUAL(strlen((const char *)trytes), 12);
  TEST_ASSERT_EQUAL_MEMORY(exp, trits, sizeof(exp));
}

void test_trit_to_tryte(void) {
  tryte_t trytes[] = {TRYTES_IN};
  trit_t trits[12];
  trit_t exp[] = {EXP};
  trytes_to_trits(trytes, trits, 4);
  TEST_ASSERT_EQUAL_MEMORY(exp, trits, sizeof(exp));
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

  RUN_TEST(test_trit_to_tryte);
  RUN_TEST(test_get_trit_at);
  RUN_TEST(test_set_trit_at);

  return UNITY_END();
}
