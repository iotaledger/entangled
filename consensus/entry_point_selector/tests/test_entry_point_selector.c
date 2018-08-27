/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/helpers/digest.h"
#include "common/model/tests/defs.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/milestone/milestone.h"
#include "consensus/tangle/tangle.h"
#include "utils/files.h"

static entry_point_selector_t eps;
static tangle_t tangle;
static milestone_t milestone;

static char* test_db_path = "consensus/entry_point_selector/tests/test.db";
static char* ciri_db_path = "consensus/entry_point_selector/tests/ciri.db";

void test_eps_init(void) {
  connection_config_t config;

  config.db_path = test_db_path;
  config.index_address = true;
  config.index_approvee = true;
  config.index_bundle = true;
  config.index_tag = true;
  config.index_hash = true;
  TEST_ASSERT(iota_tangle_init(&tangle, &config) == RC_OK);
  TEST_ASSERT(iota_consensus_entry_point_selector_init(&eps, &milestone,
                                                       &tangle, true) == RC_OK);
}

void test_eps_destroy(void) {
  TEST_ASSERT(iota_tangle_destroy(&tangle) == RC_OK);
  TEST_ASSERT(iota_consensus_entry_point_selector_destroy(&eps) == RC_OK);
}

void test_setup() {
  TEST_ASSERT(copy_file(test_db_path, ciri_db_path) == RC_OK);
  RUN_TEST(test_eps_init);
}
void test_cleanup() {
  TEST_ASSERT(remove_file(test_db_path) == RC_OK);
  RUN_TEST(test_eps_destroy);
}

void test_entry_point_with_tangle_data() {
  test_setup();
  test_cleanup();
}

void test_entry_point_without_tangle_data() {
  test_setup();
  test_cleanup();
}

int main() {
  UNITY_BEGIN();

  if (TEST_PROTECT()) {
    RUN_TEST(test_entry_point_with_tangle_data);
    RUN_TEST(test_entry_point_without_tangle_data);
  }

  return UNITY_END();
}
