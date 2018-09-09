/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <unity/unity.h>

#include "common/trinary/flex_trit.h"

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

void test_flex_trit_to_trytes(void) {
  flex_trit_t trits[] = {TRITS_IN};
  tryte_t trytes[] = {TRYTES};
  trit_t trytes_out[6];
  flex_trits_to_trytes(trytes_out, 6, trits, NUM_TRITS, NUM_TRITS);
  TEST_ASSERT_EQUAL_MEMORY(trytes, trytes_out, 6);
}

void test_flex_trit_from_trytes(void) {
  flex_trit_t trits[] = {TRITS_IN};
  tryte_t trytes[] = {TRYTES};
  trit_t trits_out[NUM_TRITS];
  flex_trits_from_trytes(trits_out, NUM_TRITS, trytes, 6, 6);
  TEST_ASSERT_EQUAL_MEMORY(trits, trits_out, sizeof(trits));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_flex_trit_to_trytes);
  RUN_TEST(test_flex_trit_from_trytes);

  return UNITY_END();
}
