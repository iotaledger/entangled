/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>
#include <unity/unity.h>

#include "common/trinary/tryte_long.h"

static const char *TRYTES[] = {"9", "A", "M", "NA", "LZ", "MM", "NNA", "N9A", "VYM", "Z", "EBN", "PCHBSYZZHYK"};

static const int64_t VALUES[] = {0, 1, 13, 14, -15, 364, 365, 716, 9418, -1, -9418, 2251799813685247};

void test_trytes_to_long(void) {
  size_t num_tests = sizeof(TRYTES) / sizeof(char *);
  for (size_t i = 0; i < num_tests; i++) {
    tryte_t *trytes = (tryte_t *)TRYTES[i];
    int64_t value = trytes_to_long(trytes, strlen((const char *)trytes));
    TEST_ASSERT_EQUAL_INT64(VALUES[i], value);
  }
}

void test_long_to_trytes(void) {
  size_t num_tests = sizeof(VALUES) / sizeof(int64_t);
  for (size_t i = 0; i < num_tests; i++) {
    int64_t value = VALUES[i];
    size_t num_trytes = min_trytes(value);
    TEST_ASSERT_EQUAL_INT(num_trytes, strlen(TRYTES[i]));
    tryte_t trytes[num_trytes];
    long_to_trytes(value, trytes);
    TEST_ASSERT_EQUAL_MEMORY(trytes, TRYTES[i], num_trytes);
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trytes_to_long);
  RUN_TEST(test_long_to_trytes);

  return UNITY_END();
}
