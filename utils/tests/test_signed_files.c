/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/consensus/snapshot/snapshot.h"
#include "utils/signed_files.h"

iota_consensus_conf_t conf;

void test_good_signature(void) {
  bool valid = false;

  TEST_ASSERT(iota_snapshot_conf_init(&conf) == RC_OK);
  TEST_ASSERT(iota_file_signature_validate(conf.snapshot_file, conf.snapshot_signature_file,
                                           conf.snapshot_signature_pubkey, conf.snapshot_signature_depth,
                                           conf.snapshot_signature_index, &valid) == RC_OK);
  TEST_ASSERT_TRUE(valid);
}

void test_bad_signature(void) {
  bool valid = false;

  TEST_ASSERT(iota_snapshot_conf_init(&conf) == RC_OK);
  TEST_ASSERT(iota_file_signature_validate(conf.snapshot_file, "utils/tests/fake.sig", conf.snapshot_signature_pubkey,
                                           conf.snapshot_signature_depth, conf.snapshot_signature_index,
                                           &valid) == RC_OK);
  TEST_ASSERT_FALSE(valid);
}

int main(void) {
  UNITY_BEGIN();

  TEST_ASSERT(iota_consensus_conf_init(&conf) == RC_OK);
  strcpy(conf.snapshot_file, SNAPSHOT_FILE);
  strcpy(conf.snapshot_conf_file, SNAPSHOT_CONF_FILE);
  strcpy(conf.snapshot_signature_file, SNAPSHOT_SIG_FILE);

  RUN_TEST(test_good_signature);
  RUN_TEST(test_bad_signature);

  return UNITY_END();
}
