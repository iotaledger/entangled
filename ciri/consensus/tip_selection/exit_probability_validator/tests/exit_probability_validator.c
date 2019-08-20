/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "ciri/consensus/conf.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "ciri/consensus/tip_selection/exit_probability_validator/exit_probability_validator.h"
#include "ciri/consensus/transaction_solidifier/transaction_solidifier.h"
#include "common/model/transaction.h"
#include "common/storage/connection.h"
#include "common/storage/storage.h"
#include "common/storage/tests/defs.h"
#include "common/trinary/flex_trit.h"

static tangle_t tangle;
static connection_config_t config;

static char *tangle_test_db_path = "ciri/consensus/tip_selection/exit_probability_validator/tests/test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static char *snapshot_path = "ciri/consensus/tip_selection/exit_probability_validator/tests/snapshot.txt";
static char *snapshot_conf_path = "ciri/consensus/snapshot/tests/snapshot_conf.json";

static uint32_t max_depth = 15;
static uint32_t max_txs_below_max_depth = 10;

static exit_prob_transaction_validator_t epv;
static snapshots_provider_t snapshots_provider;
static milestone_tracker_t mt;
static ledger_validator_t lv;
static transaction_solidifier_t ts;
static iota_consensus_conf_t consensus_conf;

void setUp() { TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK); }

void tearDown() { TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK); }

static void init_epv(exit_prob_transaction_validator_t *const epv) {
  iota_consensus_conf_init(&consensus_conf);
  consensus_conf.max_depth = max_depth;
  consensus_conf.below_max_depth = max_txs_below_max_depth;

  strcpy(consensus_conf.snapshot_file, snapshot_path);
  strcpy(consensus_conf.snapshot_conf_file, snapshot_conf_path);
  consensus_conf.snapshot_signature_skip_validation = true;

  // Avoid complete initialization with state file loading
  TEST_ASSERT(iota_snapshot_reset(&snapshots_provider.initial_snapshot, &consensus_conf) == RC_OK);
  TEST_ASSERT(iota_snapshot_reset(&snapshots_provider.latest_snapshot, &consensus_conf) == RC_OK);
  TEST_ASSERT(iota_consensus_transaction_solidifier_init(&ts, &consensus_conf, NULL, &snapshots_provider, NULL) ==
              RC_OK);
  TEST_ASSERT(iota_milestone_tracker_init(&mt, &consensus_conf, &snapshots_provider, &lv, &ts) == RC_OK);
  TEST_ASSERT(iota_consensus_ledger_validator_init(&lv, &tangle, &consensus_conf, &mt) == RC_OK);
  // We want to avoid unnecessary validation
  mt.snapshots_provider->latest_snapshot.metadata.index = 99999999999;

  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_init(&consensus_conf, &mt, &lv, epv) == RC_OK);
}

static void destroy_epv(exit_prob_transaction_validator_t *epv) {
  iota_consensus_ledger_validator_destroy(epv->lv);
  iota_milestone_tracker_destroy(&mt);
  iota_consensus_transaction_solidifier_destroy(&ts);
  iota_consensus_exit_prob_transaction_validator_destroy(epv);
  iota_snapshots_provider_destroy(&snapshots_provider);
}

void test_transaction_does_not_exist() {
  init_epv(&epv);

  bool is_valid = false;
  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  iota_transaction_t *test_tx = transaction_deserialize(tx_test_trits, true);

  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(&epv, &tangle, transaction_hash(test_tx),
                                                                      &is_valid, true) == RC_OK);
  TEST_ASSERT(!is_valid);

  destroy_epv(&epv);
  transaction_free(test_tx);
}

void test_transaction_not_a_tail() {
  init_epv(&epv);

  bool exist = false;
  bool is_valid = false;

  flex_trit_t transaction_3_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_3_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_3_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx3 = transaction_deserialize(transaction_3_trits, true);

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx3) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, transaction_hash(tx3), true) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(&epv, &tangle, transaction_hash(tx3), &is_valid,
                                                                      true) == RC_OK);
  TEST_ASSERT(!is_valid);
  transaction_free(tx3);
  destroy_epv(&epv);
}

void test_transaction_invalid_delta() {
  init_epv(&epv);

  bool exist = false;
  bool is_valid = false;

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t *)TX_1_OF_2,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx1 = transaction_deserialize(transaction_1_trits, true);

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, transaction_hash(tx1), true) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(&epv, &tangle, transaction_hash(tx1), &is_valid,
                                                                      true) == RC_OK);
  TEST_ASSERT(!is_valid);

  transaction_free(tx1);
  destroy_epv(&epv);
}

void test_transaction_below_max_depth() {
  init_epv(&epv);

  bool exist = false;
  bool is_valid = false;

  iota_transaction_t *txs[2];

  tryte_t const *const trytes[2] = {TX_1_OF_2, TX_2_OF_2};
  transactions_deserialize(trytes, txs, 2, true);
  build_tangle(&tangle, txs, 2);

  TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, transaction_hash(txs[0]), max_depth + 1) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, transaction_hash(txs[0]), true) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, transaction_hash(txs[1]), true) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(&epv, &tangle, transaction_hash(txs[0]),
                                                                      &is_valid, true) == RC_OK);
  TEST_ASSERT(!is_valid);

  transactions_free(txs, 2);
  destroy_epv(&epv);
}

void test_transaction_exceed_max_transactions() {
  max_txs_below_max_depth = 0;
  init_epv(&epv);

  bool exist = false;
  bool is_valid = false;

  iota_transaction_t *txs[2];

  tryte_t const *const trytes[2] = {TX_1_OF_2, TX_2_OF_2};
  transactions_deserialize(trytes, txs, 2, true);
  transaction_set_trunk(txs[1], consensus_conf.genesis_hash);
  transaction_set_branch(txs[1], consensus_conf.genesis_hash);
  transaction_set_branch(txs[0], consensus_conf.genesis_hash);
  build_tangle(&tangle, txs, 2);

  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, txs[0]->consensus.hash, true) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, txs[1]->consensus.hash, true) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(&epv, &tangle, transaction_hash(txs[0]),
                                                                      &is_valid, true) == RC_OK);
  TEST_ASSERT(!is_valid);

  transactions_free(txs, 2);
  destroy_epv(&epv);
  max_txs_below_max_depth = 15;
}

void test_transaction_valid() {
  init_epv(&epv);

  bool exist = false;
  bool is_valid = false;

  iota_transaction_t *txs[2];

  tryte_t const *const trytes[2] = {TX_1_OF_2, TX_2_OF_2};
  transactions_deserialize(trytes, txs, 2, true);
  transaction_set_branch(txs[0], consensus_conf.genesis_hash);
  transaction_set_branch(txs[1], consensus_conf.genesis_hash);
  transaction_set_trunk(txs[1], consensus_conf.genesis_hash);
  build_tangle(&tangle, txs, 2);

  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, transaction_hash(txs[0]), true) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, transaction_hash(txs[1]), true) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  epv.mt->latest_solid_milestone_index = max_depth;
  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(&epv, &tangle, transaction_hash(txs[0]),
                                                                      &is_valid, true) == RC_OK);
  TEST_ASSERT(is_valid);
  epv.mt->latest_solid_milestone_index = 0;
  transactions_free(txs, 2);
  destroy_epv(&epv);
}

int main() {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;

  memset(consensus_conf.genesis_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);

  RUN_TEST(test_transaction_does_not_exist);
  RUN_TEST(test_transaction_not_a_tail);
  RUN_TEST(test_transaction_invalid_delta);
  RUN_TEST(test_transaction_below_max_depth);
  RUN_TEST(test_transaction_exceed_max_transactions);
  RUN_TEST(test_transaction_valid);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
