/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "common/model/transaction.h"
#include "common/storage/connection.h"
#include "common/storage/sql/defs.h"
#include "common/storage/storage.h"
#include "common/storage/tests/helpers/defs.h"
#include "common/trinary/flex_trit.h"
#include "consensus/conf.h"
#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "consensus/tangle/tangle.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"
#include "consensus/transaction_solidifier/transaction_solidifier.h"

static tangle_t tangle;
static connection_config_t config;

static bool debug_mode = false;

static char *test_db_path =
    "consensus/exit_probability_validator/tests/test.db";
static char *ciri_db_path =
    "consensus/exit_probability_validator/tests/ciri.db";
static char *snapshot_path =
    "consensus/exit_probability_validator/tests/snapshot.txt";
static char *snapshot_conf_path = "consensus/snapshot/tests/snapshot_conf.json";

static uint32_t max_depth = 15;
static uint32_t max_txs_below_max_depth = 10;

static exit_prob_transaction_validator_t epv;
static snapshot_t snapshot;
static milestone_tracker_t mt;
static ledger_validator_t lv;
static transaction_solidifier_t ts;
static requester_state_t tr;
static iota_consensus_conf_t consensus_conf;
static iota_gossip_conf_t gossip_conf;

static void init_epv(exit_prob_transaction_validator_t *const epv) {
  iota_gossip_conf_init(&gossip_conf);
  iota_consensus_conf_init(&consensus_conf);
  consensus_conf.max_depth = max_depth;
  consensus_conf.below_max_depth = max_txs_below_max_depth;

  strcpy(consensus_conf.snapshot_file, snapshot_path);
  strcpy(consensus_conf.snapshot_conf_file, snapshot_conf_path);
  strcpy(consensus_conf.snapshot_sig_file, "");
  TEST_ASSERT(iota_snapshot_init(&snapshot, &consensus_conf) == RC_OK);
  iota_consensus_transaction_solidifier_init(&ts, &consensus_conf, &tangle,
                                             NULL);
  TEST_ASSERT(iota_milestone_tracker_init(&mt, &consensus_conf, &tangle,
                                          &snapshot, &lv, &ts) == RC_OK);
  TEST_ASSERT(requester_init(&tr, &gossip_conf, &tangle) == RC_OK);
  TEST_ASSERT(iota_consensus_ledger_validator_init(&lv, &consensus_conf,
                                                   &tangle, &mt, &tr) == RC_OK);
  // We want to avoid unnecessary validation
  mt.latest_snapshot->index = 99999999999;

  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_init(
                  &consensus_conf, &tangle, &mt, &lv, epv) == RC_OK);
}

static void destroy_epv(exit_prob_transaction_validator_t *epv) {
  iota_consensus_ledger_validator_destroy(epv->lv);
  iota_snapshot_destroy(&snapshot);
  iota_milestone_tracker_destroy(&mt);
  iota_consensus_transaction_solidifier_destroy(&ts);
  iota_consensus_exit_prob_transaction_validator_destroy(epv);
  requester_destroy(&tr);
}

void test_transaction_does_not_exist() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  init_epv(&epv);

  bool is_valid = false;
  trit_array_p tail = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail, (flex_trit_t *)TEST_TRANSACTION.hash,
                       NUM_TRITS_HASH);
  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(
                  &epv, tail, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  trit_array_free(tail);
  destroy_epv(&epv);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_transaction_not_a_tail() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  init_epv(&epv);

  bool exist = false;

  flex_trit_t transaction_3_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_3_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_3_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx3 = transaction_deserialize(transaction_3_trits);

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx3) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  bool is_valid = false;
  trit_array_p tail = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail, (flex_trit_t *)tx3->hash, NUM_TRITS_HASH);
  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(
                  &epv, tail, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  trit_array_free(tail);
  transaction_free(tx3);
  destroy_epv(&epv);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_transaction_invalid_delta() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  init_epv(&epv);

  bool exist = false;

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         (tryte_t *)TX_1_OF_2,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  tx1->snapshot_index = 0;

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  bool is_valid = false;
  trit_array_p tail = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail, (flex_trit_t *)tx1->hash, NUM_TRITS_HASH);
  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(
                  &epv, tail, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  trit_array_free(tail);
  transaction_free(tx1);
  destroy_epv(&epv);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_transaction_below_max_depth() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  init_epv(&epv);

  bool exist = false;

  iota_transaction_t txs[2];

  tryte_t const *const trytes[2] = {TX_1_OF_2, TX_2_OF_2};
  transactions_deserialize(trytes, txs, 2);
  txs[0]->snapshot_index = max_depth - 1;
  build_tangle(&tangle, txs, 2);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  bool is_valid = false;
  trit_array_p tail = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail, (flex_trit_t *)txs[0]->hash, NUM_TRITS_HASH);
  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(
                  &epv, tail, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  trit_array_free(tail);
  transactions_free(txs, 2);
  destroy_epv(&epv);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_transaction_exceed_max_transactions() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  max_txs_below_max_depth = 0;
  init_epv(&epv);

  bool exist = false;

  iota_transaction_t txs[2];

  tryte_t const *const trytes[2] = {TX_1_OF_2, TX_2_OF_2};
  transactions_deserialize(trytes, txs, 2);
  txs[0]->snapshot_index = max_depth + 1;
  txs[1]->snapshot_index = max_depth + 1;
  memcpy(txs[1]->trunk, consensus_conf.genesis_hash, FLEX_TRIT_SIZE_243);
  memcpy(txs[1]->branch, consensus_conf.genesis_hash, FLEX_TRIT_SIZE_243);
  build_tangle(&tangle, txs, 2);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  bool is_valid = false;
  trit_array_p tail = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail, (flex_trit_t *)txs[0]->hash, NUM_TRITS_HASH);
  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(
                  &epv, tail, &is_valid) == RC_OK);
  TEST_ASSERT(is_valid);

  trit_array_free(tail);
  transactions_free(txs, 2);
  destroy_epv(&epv);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  max_txs_below_max_depth = 15;
}

void test_transaction_is_genesis() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  init_epv(&epv);

  bool exist = false;

  iota_transaction_t txs[2];

  tryte_t const *const trytes[2] = {TX_1_OF_2, TX_2_OF_2};
  transactions_deserialize(trytes, txs, 2);
  txs[0]->snapshot_index = 0;
  build_tangle(&tangle, txs, 2);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  bool is_valid = false;
  trit_array_p tail = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail, (flex_trit_t *)txs[0]->hash, NUM_TRITS_HASH);
  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(
                  &epv, tail, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  trit_array_free(tail);
  transactions_free(txs, 2);
  destroy_epv(&epv);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_transaction_valid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  init_epv(&epv);

  bool exist = false;

  iota_transaction_t txs[2];

  tryte_t const *const trytes[2] = {TX_1_OF_2, TX_2_OF_2};
  transactions_deserialize(trytes, txs, 2);
  build_tangle(&tangle, txs, 2);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);

  bool is_valid = false;
  trit_array_p tail = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail, (flex_trit_t *)txs[0]->hash, NUM_TRITS_HASH);
  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_is_valid(
                  &epv, tail, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  trit_array_free(tail);
  transactions_free(txs, 2);
  destroy_epv(&epv);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

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

  memset(consensus_conf.genesis_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);

  RUN_TEST(test_transaction_does_not_exist);
  RUN_TEST(test_transaction_not_a_tail);
  RUN_TEST(test_transaction_invalid_delta);
  RUN_TEST(test_transaction_below_max_depth);
  RUN_TEST(test_transaction_exceed_max_transactions);
  RUN_TEST(test_transaction_is_genesis);
  RUN_TEST(test_transaction_valid);

  return UNITY_END();
}
