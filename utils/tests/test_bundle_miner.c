/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include <unity/unity.h>

#include "common/crypto/iss/normalize.h"
#include "common/defs.h"
#include "common/trinary/trit_tryte.h"
#include "common/trinary/trits.h"
#include "common/trinary/tryte.h"
#include "utils/bundle_miner.h"

static size_t const N = 2;
static size_t const L = N * NORMALIZED_FRAGMENT_LENGTH;
static int8_t MIN_ARRAY[L];
static int8_t MAX_ARRAY[L];

static void test_bundle_miner_probability_of_losing(void) {
  double min_probability = bundle_miner_probability_of_losing(MIN_ARRAY, N);
  double max_probability = bundle_miner_probability_of_losing(MAX_ARRAY, N);

  // TODO
  // TEST_ASSERT_EQUAL_DOUBLE(min_probability, 0.0);
  // TEST_ASSERT_EQUAL_DOUBLE(max_probability, 1.0);
}

static void test_bundle_miner_security_level(void) {
  double security = bundle_miner_security_level(bundle_miner_probability_of_losing(MAX_ARRAY, N), 3.0);

  // TODO
  // TEST_ASSERT_EQUAL_DOUBLE(security, 0.0);
}

static void test_bundle_miner_normalize_hash(void) {
  tryte_t const* const bundle_trytes =
      (tryte_t const* const) "QVXRKNRXFZIPFPREXRAPNHNSRFFQOWBGCAFZEGFCKDPDXRNVZQ9VJPQPPTFXKPVZVAIENQLETXRVSFKFO";
  trit_t bundle_trits[HASH_LENGTH_TRIT];
  byte_t bundle_expected[NORMALIZED_BUNDLE_LENGTH] = {
      13, 13, 13, 13, 13, -2, -9, -3, 6,  -1, 9, -11, 6, -11, -9, 5,   -3, -9, 1,  -11, -13, 8,  -13, -8, -9, 6,  6,
      13, -9, -4, 2,  7,  3,  1,  6,  -1, 5,  7, 6,   3, 11,  4,  -11, 4,  -3, -9, -13, -5,  -1, -10, 0,  -5, 10, -11,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,   0, 0,   0,  0,   0,  0,  0,  0,   0,   0,  0,   0,  0,  0,  0};
  byte_t bundle_actual[NORMALIZED_BUNDLE_LENGTH];

  trytes_to_trits(bundle_trytes, bundle_trits, HASH_LENGTH_TRYTE);
  normalize_hash(bundle_trits, bundle_actual);

  TEST_ASSERT_EQUAL_MEMORY(bundle_expected, bundle_actual, L);
}

static void test_bundle_miner_normalized_bundle_max(void) {
  byte_t a[L] = {0};
  byte_t b[L] = {0};
  byte_t expected[L] = {0};
  byte_t actual[L] = {0};

  a[0] = 13;
  b[3] = 12;

  expected[0] = 13;
  expected[3] = 12;

  bundle_miner_normalized_bundle_max(a, b, actual, L);

  TEST_ASSERT_EQUAL_MEMORY(expected, actual, L);
}

static void test_bundle_miner_mine(void) {
  tryte_t const* const v1_trytes =
      (tryte_t const* const) "QVXRKNRXFZIPFPREXRAPNHNSRFFQOWBGCAFZEGFCKDPDXRNVZQ9VJPQPPTFXKPVZVAIENQLETXRVSFKFO";
  trit_t v1_trits[HASH_LENGTH_TRIT];
  byte_t nb1[NORMALIZED_BUNDLE_LENGTH];
  tryte_t const* const v2_trytes =
      (tryte_t const* const) "JKHLAKTRTDIKMTERIRYEWI9PPOJAKHZEMNCXFB9GTRZRWKSFVAZANHSPABGGQIJAVULKMPPAL9VBSRB9E";
  trit_t v2_trits[HASH_LENGTH_TRIT];
  byte_t nb2[NORMALIZED_BUNDLE_LENGTH];
  byte_t min[NORMALIZED_BUNDLE_LENGTH];
  size_t const E = 486 * 4;
  trit_t essence[E];
  uint64_t index = 0;

  trytes_to_trits(v1_trytes, v1_trits, HASH_LENGTH_TRYTE);
  memset(nb1, 0, NORMALIZED_BUNDLE_LENGTH);
  trytes_to_trits(v2_trytes, v2_trits, HASH_LENGTH_TRYTE);
  memset(nb2, 0, NORMALIZED_BUNDLE_LENGTH);
  memset(min, 0, NORMALIZED_BUNDLE_LENGTH);
  memset(essence, 0, E);

  normalize_hash(v1_trits, nb1);
  normalize_hash(v2_trits, nb2);

  bundle_miner_normalized_bundle_max(nb1, nb2, min, NORMALIZED_BUNDLE_LENGTH);

  TEST_ASSERT(bundle_miner_mine(min, N, essence, E, 1000000, 0, &index) == RC_OK);

  TEST_ASSERT_EQUAL_UINT64(index, 410235);
}

int main(void) {
  UNITY_BEGIN();

  memset(MIN_ARRAY, TRYTE_VALUE_MIN, L);
  memset(MAX_ARRAY, TRYTE_VALUE_MAX, L);

  RUN_TEST(test_bundle_miner_probability_of_losing);
  RUN_TEST(test_bundle_miner_security_level);
  RUN_TEST(test_bundle_miner_normalize_hash);
  RUN_TEST(test_bundle_miner_normalized_bundle_max);
  RUN_TEST(test_bundle_miner_mine);

  return UNITY_END();
}
