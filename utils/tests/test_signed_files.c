/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "consensus/snapshot/snapshot.h"
#include "utils/signed_files.h"

void test_good_signature(void) {
  bool valid = false;
  TEST_ASSERT(
      is_file_signature_valid(MAINNET_SNAPSHOT_FILE, MAINNET_SNAPSHOT_SIG_FILE,
                              (tryte_t *)SNAPSHOT_PUBKEY, SNAPSHOT_PUBKEY_DEPTH,
                              SNAPSHOT_INDEX, &valid) == RC_OK);
  TEST_ASSERT_TRUE(valid);
}

void test_bad_signature(void) {
  bool valid = false;
  TEST_ASSERT(
      is_file_signature_valid(MAINNET_SNAPSHOT_FILE, TESTNET_SNAPSHOT_SIG_FILE,
                              (tryte_t *)SNAPSHOT_PUBKEY, SNAPSHOT_PUBKEY_DEPTH,
                              SNAPSHOT_INDEX, &valid) == RC_OK);
  TEST_ASSERT_FALSE(valid);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_good_signature);
  RUN_TEST(test_bad_signature);

  return UNITY_END();
}
