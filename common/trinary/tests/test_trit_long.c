/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>
#include <unity/unity.h>

#include "common/trinary/trit_long.h"

#define LONG_EXP 565253
#define TRITS_IN -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1
#define TRITS_IN_ENC 1, 0, -1, -1, 0, -1, 1, 0, -1, 1, -1, 0, 1, 0, 0, -1, 1, 1, 0
#define TRITS_IN_ENC_LEN 19

#define MAX_LONG_EXP 0x7FFFFFFFFFFFFFFF
#define MAX_TRITS_IN                                                                                                   \
  1, -1, 0, 1, -1, 0, -1, 1, 1, 0, -1, 0, -1, 0, 1, 0, 1, 0, -1, 1, 1, -1, -1, 1, 0, 1, -1, 0, 0, 1, 1, 1, 0, 0, 1, 1, \
      1, -1, 1, -1, 1
#define MAX_TRITS_IN_LEN 41

#define MIN_LONG_EXP 0x8000000000000000
#define MIN_TRITS_IN                                                                                                \
  1, 0, 0, -1, 1, 0, 1, -1, -1, 0, 1, 0, 1, 0, -1, 0, -1, 0, 1, -1, -1, 1, 1, -1, 0, -1, 1, 0, 0, -1, -1, -1, 0, 0, \
      -1, -1, -1, 1, -1, 1, -1
#define MIN_TRITS_IN_LEN 41

void test_min_trits(void) {
  trit_t trits[] = {TRITS_IN};
  size_t trits_size = sizeof(trits) / sizeof(trit_t);

  TEST_ASSERT_EQUAL_INT(trits_size, min_trits(LONG_EXP));
  TEST_ASSERT_EQUAL_INT(MAX_TRITS_IN_LEN, min_trits(MAX_LONG_EXP));
  TEST_ASSERT_EQUAL_INT(MIN_TRITS_IN_LEN, min_trits(MIN_LONG_EXP));
}

void test_trit_to_long(void) {
  trit_t trits[] = {TRITS_IN};
  size_t trits_size = sizeof(trits) / sizeof(trit_t);
  int64_t exp = LONG_EXP;
  int64_t val = trits_to_long(trits, trits_size);

  TEST_ASSERT_EQUAL_INT64(exp, val);
}

void test_trit_to_max_long(void) {
  trit_t trits[] = {MAX_TRITS_IN};
  size_t trits_size = sizeof(trits) / sizeof(trit_t);
  int64_t exp = MAX_LONG_EXP;
  int64_t val = trits_to_long(trits, trits_size);

  TEST_ASSERT_EQUAL_INT64(exp, val);
}

void test_trit_to_min_long(void) {
  trit_t trits[] = {MIN_TRITS_IN};
  size_t trits_size = sizeof(trits) / sizeof(trit_t);
  int64_t exp = MIN_LONG_EXP;
  int64_t val = trits_to_long(trits, trits_size);

  TEST_ASSERT_EQUAL_INT64(exp, val);
}

void test_max_long_to_trits(void) {
  size_t trits_size = min_trits(MAX_LONG_EXP);
  TEST_ASSERT_EQUAL_INT(MAX_TRITS_IN_LEN, trits_size);

  trit_t trits[trits_size];
  trit_t exp[] = {MAX_TRITS_IN};
  long_to_trits(MAX_LONG_EXP, trits);

  TEST_ASSERT_EQUAL_MEMORY(exp, trits, sizeof(trits));
}

void test_min_long_to_trits(void) {
  size_t trits_size = min_trits(MIN_LONG_EXP);
  TEST_ASSERT_EQUAL_INT(MIN_TRITS_IN_LEN, trits_size);

  trit_t trits[trits_size];
  trit_t exp[] = {MIN_TRITS_IN};
  long_to_trits(MIN_LONG_EXP, trits);

  TEST_ASSERT_EQUAL_MEMORY(exp, trits, sizeof(trits));
}

void test_from_long(void) {
  trit_t trits[min_trits(LONG_EXP)];
  trit_t exp[] = {TRITS_IN};
  long_to_trits(LONG_EXP, trits);

  TEST_ASSERT_EQUAL_MEMORY(exp, trits, sizeof(trits));
}

void test_encoded_long_length(void) { TEST_ASSERT_EQUAL_INT(TRITS_IN_ENC_LEN, encoded_length(LONG_EXP)); }

void test_encoded_long(void) {
  size_t size = encoded_length(LONG_EXP);
  trit_t trits[size];
  memset(trits, 0, sizeof(trits));
  trit_t exp[] = {TRITS_IN_ENC};

  TEST_ASSERT_EQUAL_INT(0, encode_long(LONG_EXP, trits, size));
  TEST_ASSERT_EQUAL_MEMORY(exp, trits, sizeof(exp));
}

void test_decode_long(void) {
  size_t size, end;
  size = encoded_length(LONG_EXP);
  trit_t trits[size];
  memset(trits, 0, sizeof(trits));
  trit_t exp[] = {TRITS_IN_ENC};
  memcpy(trits, exp, sizeof(exp));

  TEST_ASSERT_EQUAL_INT(LONG_EXP, decode_long(trits, size, &end));
  TEST_ASSERT_EQUAL_INT(TRITS_IN_ENC_LEN, end);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_min_trits);

  RUN_TEST(test_trit_to_long);
  RUN_TEST(test_trit_to_max_long);
  RUN_TEST(test_trit_to_min_long);

  RUN_TEST(test_from_long);
  RUN_TEST(test_max_long_to_trits);
  RUN_TEST(test_min_long_to_trits);

  RUN_TEST(test_encoded_long_length);
  RUN_TEST(test_encoded_long);
  RUN_TEST(test_decode_long);

  return UNITY_END();
}
