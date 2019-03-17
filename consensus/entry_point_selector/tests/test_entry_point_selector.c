/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/helpers/digest.h"
#include "common/model/milestone.h"
#include "common/model/tests/defs.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/tangle/tangle.h"
#include "consensus/test_utils/tangle.h"
#include "utils/files.h"

#define START_MILESTONE 814854
#define LATEST_SOLID_MILESTONE 814904
#define DEPTH 10

static entry_point_selector_t eps;
static tangle_t tangle;
static milestone_tracker_t mt;
connection_config_t config;

// gdb --args ./test_cw_ratings_dfs 1
static bool debug_mode = false;

static char* test_db_path = "consensus/entry_point_selector/tests/test.db";
static char* ciri_db_path = "consensus/entry_point_selector/tests/ciri.db";

void test_entry_point() {
  iota_milestone_t milestone = {START_MILESTONE, {0}};
  DECLARE_PACK_SINGLE_MILESTONE(ep_milestone, ep_milestone_ptr, pack);

  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_entry_point_selector_init(&eps, &mt) == RC_OK);

  mt.latest_solid_subtangle_milestone_index = LATEST_SOLID_MILESTONE;

  for (size_t i = 0; i < 100; i++) {
    TEST_ASSERT(iota_tangle_milestone_store(&tangle, &milestone) == RC_OK);
    milestone.index++;
    milestone.hash[0]++;
  }

  flex_trit_t ep[FLEX_TRIT_SIZE_243];
  TEST_ASSERT(iota_consensus_entry_point_selector_get_entry_point(&eps, &tangle, DEPTH, ep) == RC_OK);

  TEST_ASSERT(iota_tangle_milestone_load(&tangle, ep, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);

  TEST_ASSERT_EQUAL_INT(ep_milestone.index, LATEST_SOLID_MILESTONE - DEPTH);

  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_entry_point_selector_destroy(&eps) == RC_OK);
}

int main(int argc, char* argv[]) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  if (argc >= 2) {
    debug_mode = true;
  }
  if (debug_mode) {
    test_db_path = "test.db";
    ciri_db_path = "ciri.db";
  }

  config.db_path = test_db_path;

  if (TEST_PROTECT()) {
    RUN_TEST(test_entry_point);
  }

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
