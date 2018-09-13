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

void test_snapshot_init() {
  TEST_ASSERT(iota_snapshot_init(&snapshot, SNAPSHOT_FILE, SNAPSHOT_SIG_FILE,
                                 true) == RC_OK);
}

void test_snapshot_destroy() {
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

int main(int argc, char* argv[]) {
  UNITY_BEGIN();

  RUN_TEST(test_snapshot_init);
  RUN_TEST(test_snapshot_destroy);

  return UNITY_END();
}
