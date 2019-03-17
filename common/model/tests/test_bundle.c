/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/crypto/iss/normalize.h"
#include "common/model/bundle.h"

static tryte_t *trytes =
    (tryte_t *)"AABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRS"
    "TUVWXYZ";

static byte_t bytes[] = {0,   1,   2,   3,  4,  5,   6,   7,   8,   9,  10, 11, 12, 13, -13, -12, -11, -10, -9, -8, -7,
                         -6,  -5,  -4,  -3, -2, -1,  0,   1,   2,   3,  4,  5,  6,  7,  8,   9,   10,  11,  12, 13, -13,
                         -12, -11, -10, -9, -8, -7,  -6,  -5,  -4,  -3, -2, -1, 0,  1,  2,   3,   4,   5,   6,  7,  8,
                         9,   10,  11,  12, 13, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4,  -3,  -2,  -1};

void test_normalized_bundle(void) {
  size_t length = strlen((char *)trytes);
  byte_t normalized_bundle_bytes[length];
  flex_trit_t bundle_flex_trits[FLEX_TRIT_SIZE_243];

  flex_trits_from_trytes(bundle_flex_trits, HASH_LENGTH_TRIT, trytes, length, length);
  normalize_flex_hash(bundle_flex_trits, normalized_bundle_bytes);
  TEST_ASSERT_EQUAL_MEMORY(bytes, normalized_bundle_bytes, length);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_normalized_bundle);

  return UNITY_END();
}
