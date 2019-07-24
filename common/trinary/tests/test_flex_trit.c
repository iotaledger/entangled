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
#define BYTES 0x23, 0x98, 0x25, 0x02
#define NUM_TRITS 18

void test_flex_trits_to_trits(void) {
  flex_trit_t ftrits[] = {TRITS_IN};
  trit_t trits[] = {TRITS_OUT};
  trit_t trits_out[18];
  flex_trits_to_trits(trits_out, 18, ftrits, 18, 18);
  TEST_ASSERT_EQUAL_MEMORY(trits, trits_out, sizeof(trits_out));
}

void test_flex_trits_from_trits(void) {
  flex_trit_t ftrits[] = {TRITS_IN};
  trit_t trits[] = {TRITS_OUT};
  size_t num_trits = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS);
  flex_trit_t trits_out[num_trits];
  flex_trits_from_trits(trits_out, 18, trits, 18, 18);
  TEST_ASSERT_EQUAL_MEMORY(ftrits, trits_out, num_trits);
}

void test_flex_trits_to_trytes(void) {
  flex_trit_t trits[] = {TRITS_IN};
  trit_t all_trits[] = {TRITS_OUT};
  trit_t partial_trits[NUM_TRITS];
  trit_t extracted_trits[NUM_TRITS];
  tryte_t trytes_out[6];
  for (int len = 0; len <= NUM_TRITS; len++) {
    memset(extracted_trits, 0, NUM_TRITS);
    memset(partial_trits, 0, NUM_TRITS);
    memcpy(partial_trits, all_trits, len);
    flex_trits_to_trytes(trytes_out, 6, trits, NUM_TRITS, len);
    trytes_to_trits(trytes_out, extracted_trits, 6);
    TEST_ASSERT_EQUAL_MEMORY(partial_trits, extracted_trits, NUM_TRITS);
  }
}

void test_flex_trits_from_trytes(void) {
  flex_trit_t trits[] = {TRITS_IN};
  tryte_t trytes[] = {TRYTES};
  size_t num_trits = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS);
  flex_trit_t trits_out[num_trits];
  flex_trits_from_trytes(trits_out, NUM_TRITS, trytes, 6, 6);
  TEST_ASSERT_EQUAL_MEMORY(trits, trits_out, num_trits);
}

void test_flex_trits_to_bytes(void) {
  flex_trit_t trits[] = {TRITS_IN};
  byte_t bytes[] = {BYTES};
  byte_t bytes_out[4];
  flex_trits_to_bytes(bytes_out, NUM_TRITS, trits, NUM_TRITS, NUM_TRITS);
  TEST_ASSERT_EQUAL_MEMORY(bytes, bytes_out, 4);
}

void test_flex_trits_from_bytes(void) {
  flex_trit_t trits[] = {TRITS_IN};
  byte_t bytes[] = {BYTES};
  size_t num_trits = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS);
  flex_trit_t trits_out[num_trits];
  flex_trits_from_bytes(trits_out, NUM_TRITS, bytes, NUM_TRITS, NUM_TRITS);
  TEST_ASSERT_EQUAL_MEMORY(trits, trits_out, num_trits);
}

void test_flex_trits_slice(void) {
  flex_trit_t trits[] = {TRITS_IN};
  trit_t all_trits[] = {TRITS_OUT};
  trit_t partial_trits[18];
  trit_t sliced_trits[18];
  size_t num_trits = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS);
  flex_trit_t trits_out[num_trits];
  for (int start = 0; start < NUM_TRITS; start++) {
    for (int len = 0; len < NUM_TRITS - start; len++) {
      memset(sliced_trits, 0, 18);
      memset(partial_trits, 0, 18);
      memset(trits_out, FLEX_TRIT_NULL_VALUE, num_trits);
      memcpy(partial_trits, all_trits + start, len);
      flex_trits_slice(trits_out, 18, trits, 18, start, len);
      flex_trits_to_trits(sliced_trits, 18, trits_out, 18, 18);
      TEST_ASSERT_EQUAL_MEMORY(partial_trits, sliced_trits, 18);
    }
  }
}

void test_flex_trits_insert() {
  flex_trit_t const fsrc[] = {TRITS_IN};
  flex_trit_t fdst[sizeof(fsrc)];
  flex_trit_t ftmp[sizeof(fsrc)];
  trit_t src[NUM_TRITS];
  trit_t dst[NUM_TRITS];
  for (size_t num_trits = 1; num_trits < 3 * NUM_TRITS_PER_FLEX_TRIT; ++num_trits) {
    for (size_t fdst_off = 0; fdst_off < NUM_TRITS_PER_FLEX_TRIT; ++fdst_off) {
      flex_trits_insert(fdst, NUM_TRITS, fsrc, NUM_TRITS, fdst_off, num_trits);
      flex_trits_slice(ftmp, NUM_TRITS, fsrc, NUM_TRITS, 0, num_trits);
      flex_trits_to_trits(src, NUM_TRITS, ftmp, NUM_TRITS, num_trits);
      flex_trits_slice(ftmp, NUM_TRITS, fdst, NUM_TRITS, fdst_off, num_trits);
      flex_trits_to_trits(dst, NUM_TRITS, ftmp, NUM_TRITS, num_trits);
      TEST_ASSERT_EQUAL_MEMORY(src, dst, num_trits);

      for (size_t fsrc_off = 0; fsrc_off < NUM_TRITS_PER_FLEX_TRIT; ++fsrc_off) {
        flex_trits_insert_from_pos(fdst, NUM_TRITS, fsrc, NUM_TRITS, fsrc_off, fdst_off, num_trits);
        flex_trits_slice(ftmp, NUM_TRITS, fsrc, NUM_TRITS, fsrc_off, num_trits);
        flex_trits_to_trits(src, NUM_TRITS, ftmp, NUM_TRITS, num_trits);
        flex_trits_slice(ftmp, NUM_TRITS, fdst, NUM_TRITS, fdst_off, num_trits);
        flex_trits_to_trits(dst, NUM_TRITS, ftmp, NUM_TRITS, num_trits);
        TEST_ASSERT_EQUAL_MEMORY(src, dst, num_trits);
      }
    }
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_flex_trits_to_trits);
  RUN_TEST(test_flex_trits_from_trits);
  RUN_TEST(test_flex_trits_to_trytes);
  RUN_TEST(test_flex_trits_from_trytes);
  RUN_TEST(test_flex_trits_to_bytes);
  RUN_TEST(test_flex_trits_from_bytes);
  RUN_TEST(test_flex_trits_slice);
  RUN_TEST(test_flex_trits_insert);

  return UNITY_END();
}
