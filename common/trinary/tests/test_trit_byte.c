/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <string.h>
#include <unity/unity.h>

#include "common/trinary/trit_byte.h"

#define TRITS_IN -1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 1, 0, 1
#define BYTES_EXP 0x23, 0x98, 0XA

void test_trit_to_byte(void) {
  trit_t trits[] = {TRITS_IN};
  size_t trits_size = sizeof(trits) / sizeof(trit_t);
  byte_t bytes[MIN_BYTES(trits_size)];
  byte_t exp[] = {BYTES_EXP};
  trits_to_bytes(trits, bytes, trits_size);
  TEST_ASSERT_EQUAL_MEMORY(exp, bytes, sizeof(exp));
}

void test_to_from_byte(void) {
  trit_t trits[] = {TRITS_IN};
  size_t trits_size = sizeof(trits) / sizeof(trit_t);
  trit_t trits_out[trits_size];
  size_t bytes_size = MIN_BYTES(trits_size);
  byte_t bytes[bytes_size];
  trits_to_bytes(trits, bytes, trits_size);
  bytes_to_trits(bytes, bytes_size, trits_out, trits_size);
  TEST_ASSERT_EQUAL_MEMORY(trits, trits_out, sizeof(trits));
}

void test_from_byte(void) {
  trit_t trits[] = {TRITS_IN};
  byte_t bytes[] = {BYTES_EXP};
  size_t trits_size = sizeof(trits) / sizeof(trit_t);
  trit_t trits_out[trits_size];
  trit_t partial_trits[trits_size];
  size_t bytes_size = MIN_BYTES(trits_size);
  for (size_t len = 0; len <= trits_size; len++) {
    memset(trits_out, 0, trits_size);
    memset(partial_trits, 0, trits_size);
    memcpy(partial_trits, trits, len);
    bytes_to_trits(bytes, bytes_size, trits_out, len);
    TEST_ASSERT_EQUAL_MEMORY(partial_trits, trits_out, trits_size);
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_trit_to_byte);
  RUN_TEST(test_to_from_byte);
  RUN_TEST(test_from_byte);

  return UNITY_END();
}
