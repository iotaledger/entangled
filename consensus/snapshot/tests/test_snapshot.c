/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

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

int main(int argc, char* argv[]) {
  UNITY_BEGIN();

  RUN_TEST(test_snapshot_init_file_not_found);
  RUN_TEST(test_snapshot_init_file_badly_formatted);
  RUN_TEST(test_snapshot_init_file_inconsistent);
  RUN_TEST(test_snapshot_init_file_invalid_supply);

  return UNITY_END();
}
