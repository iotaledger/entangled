/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/model/transaction.h"
#include "consensus/snapshot/snapshot.h"

#define SNAPSHOT_FILE "consensus/snapshot/tests/snapshot.txt"
#define SNAPSHOT_SIG_FILE "consensus/snapshot/tests/snapshot.sig"

snapshot_t snapshot;

void test_snapshot_init_file_not_found() {
  TEST_ASSERT(iota_snapshot_init(
                  &snapshot, "consensus/snapshot/tests/snapshot_not_found.txt",
                  "consensus/snapshot/tests/snapshot_not_found.sig",
                  true) == RC_SNAPSHOT_FILE_NOT_FOUND);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_init_file_badly_formatted() {
  TEST_ASSERT(iota_snapshot_init(
                  &snapshot,
                  "consensus/snapshot/tests/snapshot_badly_formatted.txt",
                  "consensus/snapshot/tests/snapshot_badly_formatted.sig",
                  true) == RC_SNAPSHOT_INVALID_FILE);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_init_file_inconsistent() {
  TEST_ASSERT(
      iota_snapshot_init(&snapshot,
                         "consensus/snapshot/tests/snapshot_inconsistent.txt",
                         "consensus/snapshot/tests/snapshot_inconsistent.sig",
                         true) == RC_SNAPSHOT_INCONSISTENT_SNAPSHOT);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_init_file_invalid_supply() {
  TEST_ASSERT(
      iota_snapshot_init(&snapshot,
                         "consensus/snapshot/tests/snapshot_invalid_supply.txt",
                         "consensus/snapshot/tests/snapshot_invalid_supply.sig",
                         true) == RC_SNAPSHOT_INVALID_SUPPLY);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_check_consistency() {
  TEST_ASSERT(iota_snapshot_init(
                  &snapshot, "consensus/snapshot/tests/snapshot.txt",
                  "consensus/snapshot/tests/snapshot.sig", true) == RC_OK);
  TEST_ASSERT(state_diff_is_consistent(&snapshot.state) == true);
  snapshot.state->value *= -1;
  TEST_ASSERT(state_diff_is_consistent(&snapshot.state) == false);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_get_balance() {
  flex_trit_t address[FLEX_TRIT_SIZE_243];
  int64_t balance;

  TEST_ASSERT(iota_snapshot_init(&snapshot, SNAPSHOT_FILE, SNAPSHOT_SIG_FILE,
                                 true) == RC_OK);
  flex_trits_from_trytes(address, NUM_TRITS_HASH,
                         (tryte_t*)"J9999999999999999999999999999999999999999999999999999"
                         "9999999999999999999999999999",
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(iota_snapshot_get_balance(&snapshot, address, &balance) == RC_OK);
  flex_trits_from_trytes(address, NUM_TRITS_HASH,
                         (tryte_t*)"Z9999999999999999999999999999999999999999999999999999"
                         "9999999999999999999999999999",
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(iota_snapshot_get_balance(&snapshot, address, &balance) ==
              RC_SNAPSHOT_BALANCE_NOT_FOUND);
  TEST_ASSERT_EQUAL_INT(balance, 3000000);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_create_and_apply_patch() {
  state_diff_t diff = NULL;
  state_diff_t patch = NULL;
  int64_t balance = 0;
  flex_trit_t hash1[FLEX_TRIT_SIZE_243];
  flex_trit_t hash2[FLEX_TRIT_SIZE_243];

  TEST_ASSERT(iota_snapshot_init(
                  &snapshot, "consensus/snapshot/tests/snapshot.txt",
                  "consensus/snapshot/tests/snapshot.sig", true) == RC_OK);
  flex_trits_from_trytes(hash1, NUM_TRITS_HASH,
                         (tryte_t*)"O99999999999999999999999999999999999999999999999999999999999999999999999999999999",
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(state_diff_add(&diff, hash1, -50) == RC_OK);
  TEST_ASSERT(iota_snapshot_create_patch(&snapshot, &diff, &patch) == RC_OK);
  TEST_ASSERT(iota_snapshot_apply_patch(&snapshot, &diff, 1) ==
              RC_SNAPSHOT_INCONSISTENT_PATCH);
  state_diff_destroy(&patch);
  flex_trits_from_trytes(hash2, NUM_TRITS_HASH,
                                     (tryte_t*)"Q99999999999999999999999999999999999999999999999999999999999999999999999999999999",
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(state_diff_add(&diff, hash2, 50) == RC_OK);
  TEST_ASSERT(iota_snapshot_create_patch(&snapshot, &diff, &patch) == RC_OK);
  TEST_ASSERT(iota_snapshot_apply_patch(&snapshot, &diff, 2) == RC_OK);
  state_diff_destroy(&patch);
  TEST_ASSERT_EQUAL_INT(iota_snapshot_get_index(&snapshot), 2);
  TEST_ASSERT(iota_snapshot_get_balance(&snapshot, hash1, &balance) == RC_OK);
  TEST_ASSERT_EQUAL_INT(balance, 10);
  TEST_ASSERT(iota_snapshot_get_balance(&snapshot, hash2, &balance) == RC_OK);
  TEST_ASSERT_EQUAL_INT(balance, 50);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
  state_diff_destroy(&diff);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(test_snapshot_init_file_not_found);
  RUN_TEST(test_snapshot_init_file_badly_formatted);
  RUN_TEST(test_snapshot_init_file_inconsistent);
  RUN_TEST(test_snapshot_init_file_invalid_supply);
  RUN_TEST(test_snapshot_check_consistency);
  RUN_TEST(test_snapshot_get_balance);
  RUN_TEST(test_snapshot_create_and_apply_patch);

  return UNITY_END();
}
