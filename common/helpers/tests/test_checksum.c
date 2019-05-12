/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include <unity/unity.h>

#include "common/helpers/checksum.h"

static void test_checksum_equality(void) {
  tryte_t const *const ADDRESS = (tryte_t*)
      "UYEEERFQYTPFAHIPXDQAQYWYMSMCLMGBTYAXLWFRFFWPYFOICOVLK9A9VYNCKK9TQUNBTARC"
      "EQXJHD9VY";
  tryte_t const *const CHECKSUM = (tryte_t*)
      "CFOHVYSWYAKMFOPCSUXAYJQFXIZCAFLJSLZQNNJMYCBEERFGZ9C9GYDZYDADZIHJMUWGIYRP"
      "XOEDEOMRC";

  char *out = iota_checksum((char *)ADDRESS, HASH_LENGTH_TRYTE, 9);
  TEST_ASSERT_EQUAL_MEMORY(out, CHECKSUM + HASH_LENGTH_TRYTE - 9, 9);
  free(out);

  out = iota_checksum((char *)ADDRESS, HASH_LENGTH_TRYTE, 0);
  TEST_ASSERT(out == NULL);
  free(out);

  out = iota_checksum((char *)ADDRESS, HASH_LENGTH_TRYTE, 3);
  TEST_ASSERT_EQUAL_MEMORY(out, CHECKSUM + HASH_LENGTH_TRYTE - 3, 3);
  free(out);

  out = iota_checksum((char *)ADDRESS, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  TEST_ASSERT_EQUAL_MEMORY(out, CHECKSUM, HASH_LENGTH_TRYTE);
  free(out);
}

static void test_flex_checksum_equality(void) {
  size_t const NUM_TRITS = 243;
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS);
  flex_trit_t *partial = (flex_trit_t *)calloc(flex_len, sizeof(flex_trit_t));

#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  const flex_trit_t ADDRESS[] = {
      0,  1,  -1, 1,  -1, 0,  -1, -1, 1,  -1, -1, 1,  -1, -1, 1,  0,  0,  -1, 0,  -1, 1,  -1, 0,  -1, 1, -1, 0,
      -1, 1,  -1, 1,  -1, -1, 0,  -1, 1,  1,  0,  0,  -1, 0,  1,  0,  0,  1,  1,  -1, -1, 0,  -1, 0,  1, 1,  0,
      -1, 0,  -1, 1,  0,  0,  -1, 0,  -1, 1,  -1, 0,  -1, -1, 0,  1,  -1, 0,  1,  1,  1,  1,  0,  -1, 1, 1,  1,
      0,  1,  0,  0,  1,  1,  1,  1,  1,  1,  -1, 1,  -1, 1,  0,  -1, 1,  -1, 1,  -1, 0,  1,  0,  0,  0, -1, 0,
      0,  1,  1,  -1, -1, 0,  0,  -1, 1,  0,  0,  -1, 0,  -1, 1,  0,  -1, 1,  -1, -1, 0,  1,  -1, -1, 1, -1, 0,
      0,  -1, 1,  0,  -1, -1, 0,  0,  1,  0,  1,  0,  0,  -1, -1, 1,  1,  -1, 0,  1,  1,  -1, 1,  1,  0, 0,  0,
      1,  0,  0,  0,  0,  0,  1,  1,  -1, 1,  -1, 0,  -1, -1, -1, 0,  1,  0,  -1, 1,  1,  -1, 1,  1,  0, 0,  0,
      -1, 1,  -1, -1, 0,  -1, 0,  1,  -1, -1, -1, -1, -1, 1,  0,  -1, 1,  -1, 1,  0,  0,  0,  0,  -1, 0, 1,  0,
      -1, -1, 1,  -1, 0,  -1, 0,  -1, 0,  1,  0,  1,  -1, 0,  1,  1,  1,  0,  0,  0,  0,  1,  1,  -1, 1, -1, 0,
  };
  const flex_trit_t CHECKSUM[] = {
      0,  1,  0,  0,  -1, 1,  0,  -1, -1, -1, 0,  1, 1,  1,  -1, 1, -1, 0,  1,  0,  -1, -1, -1, 0,  1,  -1, 0,
      1,  0,  0,  -1, 1,  1,  1,  1,  1,  0,  -1, 1, 0,  -1, -1, 1, -1, -1, 0,  1,  0,  1,  0,  -1, 0,  1,  -1,
      0,  -1, 0,  1,  0,  0,  1,  -1, 0,  1,  0,  1, -1, 0,  -1, 0, -1, 1,  0,  -1, 0,  0,  0,  1,  -1, 0,  0,
      0,  1,  0,  1,  0,  0,  0,  -1, 1,  0,  1,  1, 1,  0,  1,  1, 0,  -1, 0,  1,  1,  -1, 0,  0,  -1, 0,  -1,
      -1, -1, -1, -1, -1, -1, 1,  0,  1,  1,  1,  1, 1,  -1, 0,  0, 1,  0,  -1, 1,  0,  -1, -1, 1,  -1, -1, 1,
      0,  0,  -1, 0,  -1, 1,  1,  -1, 1,  -1, 0,  0, 0,  0,  0,  0, 1,  0,  0,  0,  0,  1,  -1, 1,  1,  -1, 0,
      1,  1,  0,  -1, 0,  0,  1,  -1, 0,  1,  1,  0, 1,  0,  0,  1, 1,  0,  -1, 0,  0,  0,  0,  1,  -1, 0,  1,
      1,  0,  1,  1,  1,  1,  0,  1,  -1, -1, -1, 0, 1,  -1, 1,  0, 0,  1,  1,  -1, 0,  0,  0,  -1, 1,  -1, -1,
      0,  -1, 0,  0,  -1, -1, -1, -1, 1,  1,  1,  0, -1, -1, 1,  0, -1, -1, 1,  1,  1,  0,  0,  -1, 0,  1,  0,
  };
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  const flex_trit_t ADDRESS[] = {
      85, 89, 69, 69, 69, 82, 70, 81, 89, 84, 80, 70, 65, 72, 73, 80, 88, 68, 81, 65, 81, 89, 87, 89, 77, 83, 77,
      67, 76, 77, 71, 66, 84, 89, 65, 88, 76, 87, 70, 82, 70, 70, 87, 80, 89, 70, 79, 73, 67, 79, 86, 76, 75, 57,
      65, 57, 86, 89, 78, 67, 75, 75, 57, 84, 81, 85, 78, 66, 84, 65, 82, 67, 69, 81, 88, 74, 72, 68, 57, 86, 89,
  };
  const flex_trit_t CHECKSUM[] = {
      67, 70, 79, 72, 86, 89, 83, 87, 89, 65, 75, 77, 70, 79, 80, 67, 83, 85, 88, 65, 89, 74, 81, 70, 88, 73, 90,
      67, 65, 70, 76, 74, 83, 76, 90, 81, 78, 78, 74, 77, 89, 67, 66, 69, 69, 82, 70, 71, 90, 57, 67, 57, 71, 89,
      68, 90, 89, 68, 65, 68, 90, 73, 72, 74, 77, 85, 87, 71, 73, 89, 82, 80, 88, 79, 69, 68, 69, 79, 77, 82, 67,
  };
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  const flex_trit_t ADDRESS[] = {
      116, -13, 125, 31,  -52, -51, -51, -35, 115, -63, 4,  -11, 76,  49,  7,  115, -13, 52,  85, 92, 17,
      84,  -43, 29,  119, 19,  48,  -44, -61, -63, 28,  -9, -12, 13,  -57, 67, 4,   95,  83,  23, 16, 0,
      117, -13, 19,  -41, 5,   -36, 51,  -3,  31,  119, 0,  19,  -33, -52, 68, 83,  1,   -44, 13,
  };
  const flex_trit_t CHECKSUM[] = {
      4,  -57, 79, 117, 19, 63,  77, 112, 85,  28, -33, 19,  49, -51, 4,   77, 52, 115, 12, 52, 16,
      1,  71,  69, 49,  53, -52, -1, 31,  85,  13, 113, 124, 31, -52, 117, 3,  64, 0,   93, 83, 12,
      77, 17,  20, 3,   52, 69,  21, -3,  116, 80, 3,   -9,  12, -1,  21,  31, 95, -63, 4,
  };
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  const flex_trit_t ADDRESS[] = {
      -60, -66, 47, -90, 52,  -64,  -92, -77, 84, -92, -69, 6,   -64,  69,  116, 100, 10,
      121, 61,  60, 8,   78,  -11,  -79, -75, 70, -22, -75, 26,  -107, 76,  34,  9,   108,
      -88, 23,  65, -80, -92, -114, -76, 7,   72, -66, -30, -17, 13,   -45, -2,
  };
  const flex_trit_t CHECKSUM[] = {
      -78, -116, -42, 25,  68,   8,    119, 19,  -103, 30,  -19, 8,    25, -29, -25, -18, 90,
      54,  39,   -23, -5,  -118, 41,   120, 79,  -75,  47,  -90, -59,  0,  3,   102, 35,  -55,
      93,  36,   -1,  106, 120,  -114, 21,  -15, -63,  -10, 41,  -104, 46, -77, 3,
  };
#endif

  flex_trit_t *checksum = iota_flex_checksum(ADDRESS, NUM_TRITS, 0);
  TEST_ASSERT(checksum == NULL);
  free(checksum);

  checksum = iota_flex_checksum(ADDRESS, NUM_TRITS, NUM_TRITS);
  TEST_ASSERT_EQUAL_MEMORY(checksum, CHECKSUM, sizeof(CHECKSUM));
  free(checksum);

  checksum = iota_flex_checksum(ADDRESS, NUM_TRITS, 9);
  memset(partial, FLEX_TRIT_NULL_VALUE, flex_len);
  flex_trits_slice(partial, NUM_TRITS, CHECKSUM, NUM_TRITS, NUM_TRITS - 9, 9);
  TEST_ASSERT_EQUAL_MEMORY(checksum, partial, NUM_FLEX_TRITS_FOR_TRITS(9));
  free(checksum);

  checksum = iota_flex_checksum(ADDRESS, NUM_TRITS, 27);
  memset(partial, FLEX_TRIT_NULL_VALUE, flex_len);
  flex_trits_slice(partial, NUM_TRITS, CHECKSUM, NUM_TRITS, NUM_TRITS - 27, 27);
  TEST_ASSERT_EQUAL_MEMORY(checksum, partial, NUM_FLEX_TRITS_FOR_TRITS(27));
  free(checksum);

  free(partial);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_checksum_equality);
  RUN_TEST(test_flex_checksum_equality);

  return UNITY_END();
}
