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
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/tangle/tangle.h"
#include "consensus/test_utils/tangle.h"
#include "utils/files.h"

static entry_point_selector_t eps;
static tangle_t tangle;
static milestone_tracker_t mt;
connection_config_t config;

// gdb --args ./test_cw_ratings_dfs 1
static bool debug_mode = false;

static char* test_db_path = "consensus/entry_point_selector/tests/test.db";
static char* ciri_db_path = "consensus/entry_point_selector/tests/ciri.db";

void test_entry_point_with_tangle_data() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_entry_point_selector_init(&eps, &mt, &tangle,
                                                       true) == RC_OK);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_entry_point_selector_destroy(&eps) == RC_OK);
}

void test_entry_point_without_tangle_data() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_entry_point_selector_init(&eps, &mt, &tangle,
                                                       true) == RC_OK);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_entry_point_selector_destroy(&eps) == RC_OK);
}

int main(int argc, char* argv[]) {
  UNITY_BEGIN();

  if (argc >= 2) {
    debug_mode = true;
  }
  if (debug_mode) {
    test_db_path = "test.db";
    ciri_db_path = "ciri.db";
  }

  config.db_path = test_db_path;
  config.index_transaction_address = true;
  config.index_transaction_approvee = true;
  config.index_transaction_bundle = true;
  config.index_transaction_tag = true;
  config.index_transaction_hash = true;
  config.index_milestone_hash = true;

  if (TEST_PROTECT()) {
    RUN_TEST(test_entry_point_with_tangle_data);
    RUN_TEST(test_entry_point_without_tangle_data);
  }

  return UNITY_END();
}
