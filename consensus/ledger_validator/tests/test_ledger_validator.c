/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "math.h"

#include "common/model/transaction.h"
#include "common/storage/connection.h"
#include "common/storage/storage.h"
#include "common/storage/tests/helpers/defs.h"

#include "common/helpers/digest.h"
#include "common/model/milestone.h"
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone_service/milestone_service.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/snapshot/snapshots_provider.h"
#include "consensus/snapshot/snapshots_service.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"
#include "consensus/transaction_solidifier/transaction_solidifier.h"
#include "utarray.h"
#include "utils/macros.h"
#include "utils/time.h"

#define TEST_SEED "THISISASEEDFORMILESTONESERVICETESTWHICHSHOULDNOTBEUSEDWITHREALTOKENSBUTONLYFORTEA"

#define TEST_MILESTONE_HASH "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"

static tangle_t tangle;
static connection_config_t config;
static iota_consensus_conf_t conf;

// gdb --args ./test_cw_ratings_dfs 1
static bool debug_mode = false;

static char *test_db_path = "consensus/ledger_validator/tests/test.db";
static char *ciri_db_path = "consensus/ledger_validator/tests/ciri.db";
static char *snapshot_path = "consensus/ledger_validator/tests/snapshot.txt";
static char *snapshot_conf_path = "consensus/ledger_validator/tests/snapshot_conf.json";

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

void setUp() { TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) == RC_OK); }

void tearDown() { TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK); }

static void init_test_structs() {
  conf.max_depth = initial_milestone_index;

  strcpy(conf.snapshot_file, snapshot_path);
  strcpy(conf.snapshot_conf_file, snapshot_conf_path);
  conf.snapshot_signature_skip_validation = true;
  strcpy(conf.db_path, test_db_path);
  conf.last_milestone = 0;
  conf.coordinator_security_level = 1;
  conf.local_snapshots.local_snapshots_is_enabled = false;
  conf.local_snapshots.min_depth = 1;

  for (size_t i = 0; i < NUM_TRITS_HASH; ++i) {
    flex_trits_set_at(conf.genesis_hash, NUM_TRITS_HASH, i, 0);
  }

  TEST_ASSERT(iota_consensus_transaction_solidifier_init(&transaction_solidifier, &conf, NULL, NULL) == RC_OK);
  TEST_ASSERT(iota_snapshots_provider_init(&snapshots_provider, &conf) == RC_OK);
  TEST_ASSERT(iota_milestone_tracker_init(&mt, &conf, &snapshots_provider, &lv, &transaction_solidifier) == RC_OK);
  TEST_ASSERT(iota_snapshots_service_init(&snapshots_service, &snapshots_provider, &mt, &conf) == RC_OK);
  TEST_ASSERT(iota_milestone_service_init(&milestone_service, &conf) == RC_OK);
}

static void destroy_test_structs() {
  iota_milestone_service_destroy(&milestone_service);
  iota_milestone_tracker_destroy(&mt);
  iota_consensus_ledger_validator_destroy(&lv);
  iota_consensus_transaction_solidifier_destroy(&transaction_solidifier);
  iota_snapshots_provider_destroy(&snapshots_provider);
  iota_snapshots_service_destroy(&snapshots_service);
}

void test_replay_two_milestones() {
  iota_milestone_t milestone;

  iota_transaction_t *txs[2];
  iota_transaction_t *tx_iter;
  iota_transaction_t milestone_transaction;

  transaction_reset(&milestone_transaction);

  flex_trit_t seed[FLEX_TRIT_SIZE_243];
  flex_trit_t milestone_hash[FLEX_TRIT_SIZE_243];
  bundle_transactions_t *bundle_1 = NULL;
  bundle_transactions_t *bundle_2 = NULL;

  bundle_transactions_t *bundle_milestone_1 = NULL;
  bundle_transactions_t *bundle_milestone_2 = NULL;

  bundle_transactions_new(&bundle_1);
  bundle_transactions_new(&bundle_2);
  bundle_transactions_new(&bundle_milestone_1);
  bundle_transactions_new(&bundle_milestone_2);
  bool exist;
  TEST_ASSERT(iota_tangle_milestone_exist(&tangle, NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  init_test_structs();

  flex_trits_from_trytes(seed, NUM_TRITS_HASH, TEST_SEED, NUM_TRITS_HASH, NUM_TRYTES_HASH);
  flex_trits_from_trytes(milestone_hash, NUM_TRITS_HASH, TEST_MILESTONE_HASH, NUM_TRITS_HASH, NUM_TRYTES_HASH);

  iota_stor_pack_t pack;
  hash_pack_init(&pack, 2);

  iota_test_utils_bundle_create_transfer(conf.genesis_hash, conf.genesis_hash, bundle_1, seed, 0, conf.genesis_hash,
                                         100);

  BUNDLE_FOREACH(bundle_1, tx_iter) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx_iter) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, tx_iter, true) == RC_OK);
  }

  iota_test_utils_bundle_create_transfer(transaction_hash(bundle_at(bundle_1, 0)),
                                         transaction_hash(bundle_at(bundle_1, 0)), bundle_milestone_1, seed, 0,
                                         conf.genesis_hash, 0);

  milestone.index = initial_milestone_index;
  memcpy(milestone.hash, transaction_hash(bundle_at(bundle_milestone_1, 0)), FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_tangle_milestone_store(&tangle, &milestone) == RC_OK);

  TEST_ASSERT(iota_tangle_milestone_exist(&tangle, milestone.hash, &exist) == RC_OK);
  TEST_ASSERT(exist);

  BUNDLE_FOREACH(bundle_milestone_1, tx_iter) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx_iter) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, tx_iter, true) == RC_OK);
  }

  iota_test_utils_bundle_create_transfer(transaction_hash(bundle_at(bundle_milestone_1, 0)),
                                         transaction_hash(bundle_at(bundle_milestone_1, 0)), bundle_milestone_2, seed,
                                         0, conf.genesis_hash, 0);

  milestone.index++;
  memcpy(milestone.hash, transaction_hash(bundle_at(bundle_milestone_2, 0)), FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_tangle_milestone_store(&tangle, &milestone) == RC_OK);

  TEST_ASSERT(iota_tangle_milestone_exist(&tangle, milestone.hash, &exist) == RC_OK);
  TEST_ASSERT(exist);

  BUNDLE_FOREACH(bundle_milestone_2, tx_iter) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx_iter) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, tx_iter, true) == RC_OK);
  }

  TEST_ASSERT(iota_consensus_ledger_validator_init(&lv, &tangle, &conf, &mt, &milestone_service) == RC_OK);

  TEST_ASSERT_EQUAL_INT(snapshots_provider.latest_snapshot.index, 0);

  mt.running = true;
  mt.latest_milestone_index = milestone.index;
  TEST_ASSERT_EQUAL_INT(update_latest_solid_subtangle_milestone(&mt, &tangle), RC_OK);
  TEST_ASSERT_EQUAL_INT64(snapshots_provider.latest_snapshot.index, 2);

  // TEST_ASSERT(iota_snapshots_service_take_snapshot(&snapshots_service,&mt) == RC_OK);

  bundle_transactions_free(&bundle_1);
  bundle_transactions_free(&bundle_2);
  bundle_transactions_free(&bundle_milestone_1);
  bundle_transactions_free(&bundle_milestone_2);
  hash_pack_free(&pack);
  destroy_test_structs();
}

void test_replay_several_milestones() {
  iota_milestone_t milestone;

  iota_transaction_t *txs[2];
  iota_transaction_t *tx_iter;
  iota_transaction_t milestone_transaction;
  flex_trit_t curr_hash[FLEX_TRIT_SIZE_243];

  transaction_reset(&milestone_transaction);
  size_t i;
  size_t num_milestones = 10;

  flex_trit_t seed[FLEX_TRIT_SIZE_243];
  flex_trit_t milestone_hash[FLEX_TRIT_SIZE_243];
  bundle_transactions_t *bundle = NULL;

  bool exist;
  TEST_ASSERT(iota_tangle_milestone_exist(&tangle, NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  init_test_structs();

  flex_trits_from_trytes(seed, NUM_TRITS_HASH, TEST_SEED, NUM_TRITS_HASH, NUM_TRYTES_HASH);
  flex_trits_from_trytes(milestone_hash, NUM_TRITS_HASH, TEST_MILESTONE_HASH, NUM_TRITS_HASH, NUM_TRYTES_HASH);

  iota_stor_pack_t pack;
  hash_pack_init(&pack, 2);

  memcpy(curr_hash, conf.genesis_hash, FLEX_TRIT_SIZE_243);
  milestone.index = 0;

  for (i = 0; i < num_milestones * 2; ++i) {
    bundle_transactions_new(&bundle);
    iota_test_utils_bundle_create_transfer(curr_hash, curr_hash, bundle, seed, 0, conf.genesis_hash, i % 2 ? 0 : 1000);
    if (i % 2) {
      // milestone bundle
      memcpy(milestone.hash, transaction_hash(bundle_at(bundle, 0)), FLEX_TRIT_SIZE_243);
      milestone.index++;
      TEST_ASSERT(iota_tangle_milestone_store(&tangle, &milestone) == RC_OK);
    }

    BUNDLE_FOREACH(bundle, tx_iter) {
      TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx_iter) == RC_OK);
      TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, tx_iter, true) == RC_OK);
    }
    memcpy(curr_hash, transaction_hash(bundle_at(bundle, 0)), FLEX_TRIT_SIZE_243);
    bundle_transactions_free(&bundle);
  }

  TEST_ASSERT(iota_consensus_ledger_validator_init(&lv, &tangle, &conf, &mt, &milestone_service) == RC_OK);

  TEST_ASSERT_EQUAL_INT(snapshots_provider.latest_snapshot.index, 0);

  mt.running = true;
  mt.latest_milestone_index = milestone.index;
  TEST_ASSERT_EQUAL_INT(update_latest_solid_subtangle_milestone(&mt, &tangle), RC_OK);
  TEST_ASSERT_EQUAL_INT64(snapshots_provider.latest_snapshot.index, num_milestones);

  // TEST_ASSERT(iota_snapshots_service_take_snapshot(&snapshots_service,&mt) == RC_OK);

  hash_pack_free(&pack);
  destroy_test_structs();
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  if (argc >= 2) {
    debug_mode = true;
  }
  if (debug_mode) {
    test_db_path = "test.db";
    ciri_db_path = "ciri.db";
    snapshot_path = "snapshot.txt";
    snapshot_conf_path = "snapshot_conf.json";
  }

  config.db_path = test_db_path;

  iota_consensus_conf_init(&conf);

  RUN_TEST(test_replay_two_milestones);
  RUN_TEST(test_replay_several_milestones);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
