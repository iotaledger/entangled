/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "math.h"

#include "ciri/consensus/ledger_validator/ledger_validator.h"
#include "ciri/consensus/milestone/milestone_service.h"
#include "ciri/consensus/milestone/milestone_tracker.h"
#include "ciri/consensus/snapshot/snapshots_provider.h"
#include "ciri/consensus/snapshot/snapshots_service.h"
#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "ciri/consensus/transaction_solidifier/transaction_solidifier.h"
#include "common/helpers/digest.h"
#include "common/model/milestone.h"
#include "common/model/transaction.h"
#include "common/storage/connection.h"
#include "common/storage/storage.h"
#include "common/storage/tests/defs.h"
#include "utarray.h"
#include "utils/containers/hash/hash_uint64_t_map.h"
#include "utils/macros.h"
#include "utils/time.h"

#define TEST_SEED "THISISASEEDFORMILESTONESERVICETESTWHICHSHOULDNOTBEUSEDWITHREALTOKENSBUTONLYFORTEA"

#define TEST_MILESTONE_HASH "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
#define TEST_NUM_SNAPSHOTS 2

static tangle_t tangle;
static storage_connection_config_t config;
static iota_consensus_conf_t conf;

static char *tangle_test_db_path = "ciri/consensus/ledger_validator/tests/test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static char *snapshot_path = "ciri/consensus/ledger_validator/tests/snapshot.txt";
static char *local_snapshot_path_base = "ciri/consensus/ledger_validator/tests/local_snapshot";
static char *snapshot_conf_path = "ciri/consensus/ledger_validator/tests/snapshot_conf.json";

static uint64_t initial_milestone_index = 1;

typedef enum test_tangle_topology {
  ONLY_DIRECT_APPROVERS,
  BLOCKCHAIN,
} test_tangle_topology;

static milestone_service_t milestone_service;
static ledger_validator_t lv;
static milestone_tracker_t mt;
static snapshots_provider_t snapshots_provider;
static snapshots_service_t snapshots_service;
static transaction_solidifier_t transaction_solidifier;
static pruning_service_t ps;
static tips_cache_t tips;
static flex_trit_t g_seed[FLEX_TRIT_SIZE_243];

void setUp() { TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK); }

void tearDown() { TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK); }

static void init_test_structs() {
  conf.max_depth = initial_milestone_index;

  strcpy(conf.snapshot_file, snapshot_path);
  strcpy(conf.snapshot_conf_file, snapshot_conf_path);
  strcpy(conf.local_snapshots.local_snapshots_path_base, local_snapshot_path_base);
  conf.snapshot_signature_skip_validation = true;
  strcpy(conf.tangle_db_path, tangle_test_db_path);
  conf.last_milestone = 0;
  conf.coordinator_security_level = 1;
  conf.local_snapshots.local_snapshots_is_enabled = false;
  conf.local_snapshots.min_depth = 1;
  conf.snapshot_signature_index = 0;
  strcpy(conf.local_snapshots.local_snapshots_path_base, local_snapshot_path_base);
  memset(conf.genesis_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  flex_trits_from_trytes(g_seed, NUM_TRITS_HASH, (tryte_t *)TEST_SEED, NUM_TRITS_HASH, NUM_TRYTES_HASH);

  conf.local_snapshots.local_snapshots_is_enabled = true;
  TEST_ASSERT(iota_snapshots_provider_init(&snapshots_provider, &conf) == RC_OK);
  TEST_ASSERT(iota_consensus_transaction_solidifier_init(&transaction_solidifier, &conf, NULL, &snapshots_provider,
                                                         NULL) == RC_OK);
  TEST_ASSERT(iota_milestone_tracker_init(&mt, &conf, &snapshots_provider, &lv, &transaction_solidifier) == RC_OK);
  TEST_ASSERT(iota_snapshots_service_init(&snapshots_service, &snapshots_provider, &milestone_service, &conf) == RC_OK);
  TEST_ASSERT(iota_milestone_service_init(&milestone_service, &conf) == RC_OK);
  TEST_ASSERT(iota_local_snapshots_pruning_service_init(&ps, &snapshots_provider, NULL, &tips, &conf) == RC_OK);
}

static void destroy_test_structs() {
  iota_milestone_service_destroy(&milestone_service);
  iota_milestone_tracker_destroy(&mt);
  iota_consensus_ledger_validator_destroy(&lv);
  iota_consensus_transaction_solidifier_destroy(&transaction_solidifier);
  iota_snapshots_provider_destroy(&snapshots_provider);
  iota_snapshots_service_destroy(&snapshots_service);
}

static void test_util_create_bundles(flex_trit_t *const curr_branch_trunk_hash, iota_milestone_t *const milestone,
                                     size_t num_milestones, uint64_t *sk_index) {
  bundle_transactions_t *bundle = NULL;
  iota_transaction_t *tx_iter;

  for (size_t i = 0; i < num_milestones * 2; ++i) {
    bundle_transactions_new(&bundle);
    iota_test_utils_bundle_create_transfer(curr_branch_trunk_hash, curr_branch_trunk_hash, bundle, g_seed,
                                           i % 2 ? *sk_index : (*sk_index)++, i % 2 ? 0 : 1000);
    if (i % 2) {
      // milestone bundle
      memcpy(milestone->hash, transaction_hash(bundle_at(bundle, 0)), FLEX_TRIT_SIZE_243);
      milestone->index++;
      TEST_ASSERT(iota_tangle_milestone_store(&tangle, milestone) == RC_OK);
    }

    BUNDLE_FOREACH(bundle, tx_iter) { TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx_iter) == RC_OK); }
    memcpy(curr_branch_trunk_hash, transaction_hash(bundle_at(bundle, 0)), FLEX_TRIT_SIZE_243);
    bundle_transactions_free(&bundle);
  }
}

static void test_snapshots_equal(snapshot_t const *const lhs, snapshot_t const *const rhs) {
  TEST_ASSERT(state_delta_equal(lhs->state, rhs->state));

  TEST_ASSERT_EQUAL_MEMORY(lhs->metadata.hash, rhs->metadata.hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT_EQUAL_INT64(lhs->metadata.index, rhs->metadata.index);
  TEST_ASSERT_EQUAL_INT64(lhs->metadata.timestamp, rhs->metadata.timestamp);
  TEST_ASSERT(hash_to_uint64_t_map_equal(lhs->metadata.solid_entry_points, rhs->metadata.solid_entry_points));
}

static void test_replay_several_milestones() {
  iota_milestone_t milestone;
  flex_trit_t curr_hash[FLEX_TRIT_SIZE_243];
  size_t num_milestones = 3;
  uint64_t sk_index = 0;

  flex_trit_t milestone_hash[FLEX_TRIT_SIZE_243];

  bool exist;
  TEST_ASSERT(iota_tangle_milestone_exist(&tangle, NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  init_test_structs();

  TEST_ASSERT_EQUAL_INT(snapshots_provider.latest_snapshot.metadata.index, 0);

  flex_trits_from_trytes(milestone_hash, NUM_TRITS_HASH, (tryte_t *)TEST_MILESTONE_HASH, NUM_TRITS_HASH,
                         NUM_TRYTES_HASH);

  memcpy(curr_hash, conf.genesis_hash, FLEX_TRIT_SIZE_243);
  milestone.index = 0;

  TEST_ASSERT(iota_consensus_ledger_validator_init(&lv, &tangle, &conf, &mt) == RC_OK);

  TEST_ASSERT_EQUAL_INT(snapshots_provider.latest_snapshot.metadata.index, 0);

  mt.running = true;

  for (size_t i = 0; i < TEST_NUM_SNAPSHOTS; ++i) {
    test_util_create_bundles(curr_hash, &milestone, num_milestones, &sk_index);

    mt.latest_milestone_index = milestone.index;
    TEST_ASSERT_EQUAL_INT(RC_OK, update_latest_solid_milestone(&mt, &tangle));
    TEST_ASSERT_EQUAL_INT64(num_milestones * (i + 1), snapshots_provider.latest_snapshot.metadata.index);
    TEST_ASSERT_EQUAL_INT(RC_OK, iota_snapshots_service_take_snapshot(&snapshots_service, &ps, &tangle));
    TEST_ASSERT_EQUAL_INT64(snapshots_provider.initial_snapshot.metadata.index,
                            (num_milestones * (i + 1) - conf.local_snapshots.min_depth - 1));
    snapshot_t tmp;
    conf.local_snapshots.local_snapshots_is_enabled = true;
    TEST_ASSERT_EQUAL_INT(RC_OK, iota_snapshot_init(&tmp, &conf));
    test_snapshots_equal(&snapshots_provider.initial_snapshot, &tmp);
    iota_snapshot_destroy(&tmp);
  }

  destroy_test_structs();
}

int main() {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;

  iota_consensus_conf_init(&conf);

  RUN_TEST(test_replay_several_milestones);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
