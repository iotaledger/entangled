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

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trit_to_tryte);

  return UNITY_END();
}
