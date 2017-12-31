#include <stdio.h>
#include <string.h>
#include <unity/unity.h>

#include "common/trinary/trit_long.h"

#define TRITS_ENC_IN 0, -1, -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1
#define TRITS_IN -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1
#define LONG_EXP 565253

void test_min_trits(void) {
  trit_t trits[] = {TRITS_IN};
  size_t trits_size = sizeof(trits) / sizeof(trit_t);
  TEST_ASSERT_EQUAL_INT(trits_size, min_trits(LONG_EXP));
}

void test_trit_to_long(void) {
  trit_t trits[] = {TRITS_IN};
  size_t trits_size = sizeof(trits) / sizeof(trit_t);
  long long exp = LONG_EXP;
  long long val = trits_to_long(trits, trits_size);
  TEST_ASSERT_EQUAL_INT64(exp, val);
}

void test_from_long(void) {
  trit_t trits[min_trits(LONG_EXP)];
  trit_t exp[] = {TRITS_IN};
  long_to_trits(LONG_EXP, trits);
  TEST_ASSERT_EQUAL_MEMORY(exp, trits, sizeof(trits));
}

void test_encoded_long_length(void) {
  TEST_ASSERT_EQUAL_INT(29, encoded_length(LONG_EXP));
}

void test_encoded_long(void) {
  size_t size = encoded_length(LONG_EXP);
  trit_t trits[size];
  memset(trits, 0, sizeof(trits));
  trit_t exp[] = {TRITS_ENC_IN};
  TEST_ASSERT_EQUAL_INT(0, encode_long(LONG_EXP, trits, size));
  TEST_ASSERT_EQUAL_MEMORY(exp, trits, sizeof(exp));
}

void test_decode_long(void) {
  size_t size, end;
  size = encoded_length(LONG_EXP);
  trit_t trits[size];
  memset(trits, 0, sizeof(trits));
  trit_t exp[] = {TRITS_ENC_IN};
  memcpy(trits, exp, sizeof(exp));

  TEST_ASSERT_EQUAL_INT(LONG_EXP, get_encoded_long(trits, size, &end));
  TEST_ASSERT_EQUAL_INT(29, end);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_min_trits);
  RUN_TEST(test_trit_to_long);
  RUN_TEST(test_from_long);

  RUN_TEST(test_encoded_long_length);
  RUN_TEST(test_encoded_long);
  RUN_TEST(test_decode_long);

  return UNITY_END();
}
