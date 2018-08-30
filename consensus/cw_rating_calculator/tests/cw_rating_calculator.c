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
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "utarray.h"
#include "utils/files.h"

static cw_rating_calculator_t calc;
static tangle_t tangle;

// gdb --args ./test_cw_ratings_dfs 1
static bool debug_mode = false;

static char *test_db_path = "consensus/cw_rating_calculator/tests/test.db";
static char *ciri_db_path = "consensus/cw_rating_calculator/tests/ciri.db";

enum TestTangleTopology {
  OnlyDirectApprovers,
  Blockchain,
};
typedef enum TestTangleTopology TestTangleTopology;

void test_init_cw(void) {
  connection_config_t config;

  config.db_path = test_db_path;
  config.index_transaction_address = true;
  config.index_transaction_approvee = true;
  config.index_transaction_bundle = true;
  config.index_transaction_tag = true;
  config.index_transaction_hash = true;
  config.index_milestone_hash = true;
  TEST_ASSERT(iota_tangle_init(&tangle, &config) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle, DFS) == RC_OK);
}

void test_setup() {
  TEST_ASSERT(copy_file(test_db_path, ciri_db_path) == RC_OK);
  RUN_TEST(test_init_cw);
}
void test_cleanup() {
  TEST_ASSERT(iota_tangle_destroy(&tangle) == RC_OK);
  TEST_ASSERT(remove_file(test_db_path) == RC_OK);
}

void test_cw_gen_topology(TestTangleTopology topology) {
  cw_entry_t *currCwEntry = NULL;
  cw_entry_t *tmpCwEntry = NULL;
  size_t numApprovers = 200;
  size_t numTxs = numApprovers + 1;

  test_setup();

  struct _iota_transaction txs[numApprovers];
  txs[0] = TEST_TRANSACTION;
  txs[0].hash[0] += 1;
  memcpy(txs[0].branch, TEST_TRANSACTION.hash, FLEX_TRIT_SIZE_243);
  for (int i = 1; i < numApprovers; i++) {
    txs[i] = TEST_TRANSACTION;
    // Different hash for each tx,
    // we don't worry about it not being valid encoding
    txs[i].hash[i / 256] += (i + 1);
    if (topology == OnlyDirectApprovers) {
      memcpy(txs[i].branch, txs[i - 1].branch, FLEX_TRIT_SIZE_243);
    } else if (topology == Blockchain) {
      memcpy(txs[i].branch, txs[i - 1].hash, FLEX_TRIT_SIZE_243);
    }
  }

  for (int i = 0; i < numApprovers; i++) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
  }

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, (flex_trit_t *)TEST_TRANSACTION.hash,
                       NUM_TRITS_HASH);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, numApprovers);

  cw_calc_result out;

  trit_array_p currHash = trit_array_new(NUM_TRITS_HASH);
  for (int i = 0; i < numApprovers; i++) {
    trit_array_set_trits(currHash, txs[i].hash, NUM_TRITS_HASH);
    TEST_ASSERT(iota_tangle_transaction_load_hashes(
                    &tangle, TRANSACTION_COL_HASH, currHash, &pack) == RC_OK);
    TEST_ASSERT_EQUAL_INT(i + 1, pack.num_loaded);
  }
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle, DFS) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), numApprovers + 1);

  size_t totalWeight = 0;

  HASH_ITER(hh, out.cw_ratings, currCwEntry, tmpCwEntry) {
    totalWeight += currCwEntry->cw;
  }
  // First Entry, for both topologies, has to equal the number of total
  // approvers + own weight
  TEST_ASSERT_EQUAL_INT(out.cw_ratings->cw, numApprovers + 1);
  if (topology == OnlyDirectApprovers) {
    TEST_ASSERT_EQUAL_INT(totalWeight, numApprovers + 1 + numApprovers);
  } else if (topology == Blockchain) {
    // Sum of series 1 + 2 + ... + (numTxs)
    TEST_ASSERT_EQUAL_INT(totalWeight, (numTxs * (numTxs + 1)) / 2);
  }

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  trit_array_free(ep);
  test_cleanup();
}

void test_single_tx_tangle(void) {
  test_setup();

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, (flex_trit_t *)TEST_TRANSACTION.hash,
                       NUM_TRITS_HASH);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, 5);

  cw_calc_result out;
  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_store(
                  &tangle, (iota_transaction_t)&TEST_TRANSACTION) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, NULL, NULL, &exist) ==
              RC_OK);
  TEST_ASSERT(exist == true);
  TEST_ASSERT(iota_tangle_transaction_load_hashes(&tangle, TRANSACTION_COL_HASH,
                                                  ep, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(1, pack.num_loaded);
  TEST_ASSERT_EQUAL_MEMORY(((trit_array_p)pack.models[0])->trits, ep->trits,
                           FLEX_TRIT_SIZE_243);

  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), 1);

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  trit_array_free(ep);
  test_cleanup();
}

void test_cw_topology_only_direct_approvers(void) {
  test_cw_gen_topology(OnlyDirectApprovers);
}
void test_cw_topology_blockchain(void) { test_cw_gen_topology(Blockchain); }

void test_cw_topology_four_transactions_diamond(void) {
  cw_entry_t *currCwEntry = NULL;
  cw_entry_t *tmpCwEntry = NULL;

  size_t numTxs = 4;

  test_setup();

  struct _iota_transaction txs[numTxs];
  txs[0] = TEST_TRANSACTION;
  for (int i = 1; i < numTxs; i++) {
    txs[i] = TEST_TRANSACTION;
    // Different hash for each tx,
    // we don't worry about it not being valid encoding
    txs[i].hash[0] += (i + 1);
  }

  /// First two transactions approve entry point.
  memcpy(txs[1].branch, txs[0].hash, FLEX_TRIT_SIZE_243);
  memcpy(txs[2].branch, txs[0].hash, FLEX_TRIT_SIZE_243);
  /// Third transaction approves first two via branch+trunk
  memcpy(txs[3].branch, txs[1].hash, FLEX_TRIT_SIZE_243);
  memcpy(txs[3].trunk, txs[2].hash, FLEX_TRIT_SIZE_243);

  for (int i = 0; i < numTxs; i++) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
  }

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, txs[0].hash, NUM_TRITS_HASH);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, numTxs);

  cw_calc_result out;

  trit_array_p currHash = trit_array_new(NUM_TRITS_HASH);
  for (int i = 0; i < numTxs; i++) {
    trit_array_set_trits(currHash, txs[i].hash, NUM_TRITS_HASH);
    TEST_ASSERT(iota_tangle_transaction_load_hashes(
                    &tangle, TRANSACTION_COL_HASH, currHash, &pack) == RC_OK);
    TEST_ASSERT_EQUAL_INT(i + 1, pack.num_loaded);
  }
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle, DFS) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), numTxs);

  size_t totalWeight = 0;

  HASH_ITER(hh, out.cw_ratings, currCwEntry, tmpCwEntry) {
    totalWeight += currCwEntry->cw;
  }

  TEST_ASSERT_EQUAL_INT(totalWeight, 4 + 2 + 2 + 1);

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  trit_array_free(ep);
  test_cleanup();
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  if (argc >= 2) {
    debug_mode = true;
  }
  if (debug_mode) {
    test_db_path = "test.db";
    ciri_db_path = "ciri.db";
  }

  RUN_TEST(test_single_tx_tangle);
  RUN_TEST(test_cw_topology_blockchain);
  RUN_TEST(test_cw_topology_only_direct_approvers);
  RUN_TEST(test_cw_topology_four_transactions_diamond);

  return UNITY_END();
}
