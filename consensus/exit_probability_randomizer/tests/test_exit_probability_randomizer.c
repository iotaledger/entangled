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
#include "common/storage/sql/defs.h"
#include "common/storage/storage.h"
#include "common/storage/tests/helpers/defs.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/exit_probability_randomizer/exit_probability_randomizer.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"
#include "utarray.h"
#include "utils/macros.h"

static cw_rating_calculator_t calc;
static tangle_t tangle;
static connection_config_t config;

// gdb --args ./test_cw_ratings_dfs 1
static bool debug_mode = false;

static char *test_db_path =
    "consensus/exit_probability_randomizer/tests/test.db";
static char *ciri_db_path =
    "consensus/exit_probability_randomizer/tests/ciri.db";
static char *snapshot_path =
    "consensus/exit_probability_randomizer/tests/snapshot.txt";
static char *snapshot_conf_path = "consensus/snapshot/tests/snapshot_conf.txt";

static double low_alpha = 0;
static double high_alpha = 1;

static uint32_t max_txs_below_max_depth = 10000;
static uint32_t max_depth = 15;

typedef enum test_tangle_topology {
  ONLY_DIRECT_APPROVERS,
  BLOCKCHAIN,
} test_tangle_topology;

static exit_prob_transaction_validator_t epv;
static snapshot_t snapshot;
static milestone_tracker_t mt;
static ledger_validator_t lv;

static void init_epv(exit_prob_transaction_validator_t *const epv) {
  TEST_ASSERT(iota_snapshot_init(&snapshot, snapshot_path, NULL,
                                 snapshot_conf_path, true) == RC_OK);
  TEST_ASSERT(iota_milestone_tracker_init(&mt, &tangle, &snapshot, &lv, true) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_ledger_validator_init(&lv, &tangle, &mt, NULL) ==
              RC_OK);

  // We want to avoid unnecessary validation
  mt.latest_snapshot->index = 99999999999;

  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_init(
                  &tangle, &mt, &lv, epv, max_txs_below_max_depth, max_depth) ==
              RC_OK);
}

static void destroy_epv(exit_prob_transaction_validator_t *epv) {
  iota_consensus_ledger_validator_destroy(epv->lv);
  iota_snapshot_destroy(&snapshot);
  iota_milestone_tracker_destroy(&mt);
  iota_consensus_exit_prob_transaction_validator_destroy(epv);
}

void test_cw_gen_topology(test_tangle_topology topology) {
  hash_to_int_map_entry_t *curr_cw_entry = NULL;
  hash_to_int_map_entry_t *tmp_cw_entry = NULL;
  size_t num_approvers = 50;
  size_t num_txs = num_approvers + 1;

  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_store(
                  &tangle, (iota_transaction_t)&TEST_TRANSACTION) == RC_OK);

  struct _iota_transaction txs[num_approvers];
  txs[0] = TEST_TRANSACTION;
  txs[0].hash[0] += 1;
  memcpy(txs[0].branch, TEST_TRANSACTION.hash, FLEX_TRIT_SIZE_243);
  for (int i = 1; i < num_approvers; i++) {
    txs[i] = TEST_TRANSACTION;
    // Different hash for each tx,
    // we don't worry about it not being valid encoding
    txs[i].hash[i / 256] += (i + 1);
    if (topology == ONLY_DIRECT_APPROVERS) {
      memcpy(txs[i].branch, txs[i - 1].branch, FLEX_TRIT_SIZE_243);
    } else if (topology == BLOCKCHAIN) {
      memcpy(txs[i].branch, txs[i - 1].hash, FLEX_TRIT_SIZE_243);
    }
  }

  for (int i = 0; i < num_approvers; i++) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
  }

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, (flex_trit_t *)TEST_TRANSACTION.hash,
                       NUM_TRITS_HASH);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, num_approvers);

  cw_calc_result out;
  trit_array_p curr_hash = trit_array_new(NUM_TRITS_HASH);
  bool exist;
  for (int i = 0; i < num_approvers; i++) {
    trit_array_set_trits(curr_hash, txs[i].hash, NUM_TRITS_HASH);
    TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_HASH,
                                              curr_hash, &exist) == RC_OK);
    TEST_ASSERT(exist);
  }
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(num_approvers + 1, HASH_COUNT(out.tx_to_approvers));

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) {
    total_weight += curr_cw_entry->value;
  }
  // First Entry, for both topologies, has to equal the number of total
  // approvers + own weight
  TEST_ASSERT_EQUAL_INT(out.cw_ratings->value, num_approvers + 1);
  if (topology == ONLY_DIRECT_APPROVERS) {
    TEST_ASSERT_EQUAL_INT(total_weight, num_approvers + 1 + num_approvers);
  } else if (topology == BLOCKCHAIN) {
    // Sum of series 1 + 2 + ... + (num_txs)
    TEST_ASSERT_EQUAL_INT(total_weight, (num_txs * (num_txs + 1)) / 2);
  }

  /// Exit Probabilities - start

  ep_randomizer_t ep_randomizer;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(&ep_randomizer, &tangle, 0,
                                                EP_RANDOM_WALK) == RC_OK);

  trit_array_t tip;
  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];
  tip.trits = tip_trits;

  init_epv(&epv);
  /// Select the tip
  uint16_t selected_tip_counts[num_approvers];
  memset(selected_tip_counts, 0, sizeof(selected_tip_counts));

  int selections = 200;
  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(
                    &ep_randomizer, &epv, &out, ep, &tip) == RC_OK);

    for (size_t a = 0; a < num_approvers; ++a) {
      if (memcmp(tip.trits, txs[a].hash, FLEX_TRIT_SIZE_243) == 0) {
        selected_tip_counts[a] += 1;
      }
    }
  }

  uint16_t total_selections = 0;
  if (topology == ONLY_DIRECT_APPROVERS) {
    // We can look on the previous trial as a sample from
    // binomial distribution where `p` = 1/num_approvers, `n` = selections,
    // so we get (mean = `np`, stdev = `np*(1-p)`):
    double expected_mean = ((double)selections / (double)num_approvers);
    double expected_stdev =
        sqrt(expected_mean * (1.0 - 1.0 / ((double)num_approvers)));
    for (size_t a = 0; a < num_approvers; ++a) {
      uint16_t comp_up = expected_mean + 5 * expected_stdev;
      uint16_t comp_low = MAX((expected_mean - 5 * expected_stdev), 0);
      TEST_ASSERT(selected_tip_counts[a] < comp_up);
      TEST_ASSERT(selected_tip_counts[a] >= comp_low);
    }
  }

  for (size_t a = 0; a < num_approvers; ++a) {
    total_selections += selected_tip_counts[a];
  }
  TEST_ASSERT(total_selections == selections);

  TEST_ASSERT(iota_consensus_ep_randomizer_destroy(&ep_randomizer) == RC_OK);

  /// Exit Probabilities - end

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  trit_array_free(ep);
  trit_array_free(curr_hash);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
}

void test_single_tx_tangle(void) {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, (flex_trit_t *)TEST_TRANSACTION.hash,
                       NUM_TRITS_HASH);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, 5);

  cw_calc_result out;
  bool exist = false;

  TEST_ASSERT(iota_tangle_transaction_store(
                  &tangle, (iota_transaction_t)&TEST_TRANSACTION) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_HASH, ep,
                                            &exist) == RC_OK);
  TEST_ASSERT(exist);

  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), 1);

  ep_randomizer_t ep_randomizer;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(&ep_randomizer, &tangle, 0.01,
                                                EP_RANDOM_WALK) == RC_OK);

  trit_array_t tip;
  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];
  tip.trits = tip_trits;

  init_epv(&epv);
  /// Select the tip
  TEST_ASSERT(iota_consensus_exit_probability_randomize(
                  &ep_randomizer, &epv, &out, ep, &tip) == RC_OK);

  /// Check that tip was selected
  TEST_ASSERT_EQUAL_MEMORY(tip.trits, ep->trits, FLEX_TRIT_SIZE_243);

  TEST_ASSERT(iota_consensus_ep_randomizer_destroy(&ep_randomizer) == RC_OK);

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  trit_array_free(ep);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
}

void test_cw_topology_only_direct_approvers(void) {
  test_cw_gen_topology(ONLY_DIRECT_APPROVERS);
}
void test_cw_topology_blockchain(void) { test_cw_gen_topology(BLOCKCHAIN); }

void test_cw_topology_four_transactions_diamond(void) {
  hash_to_int_map_entry_t *curr_cw_entry = NULL;
  hash_to_int_map_entry_t *tmp_cw_entry = NULL;

  size_t num_txs = 4;

  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);

  bool exist;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  struct _iota_transaction txs[num_txs];
  txs[0] = TEST_TRANSACTION;
  for (int i = 1; i < num_txs; i++) {
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

  for (int i = 0; i < num_txs; i++) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
  }

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, txs[0].hash, NUM_TRITS_HASH);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, num_txs);

  cw_calc_result out;

  trit_array_p curr_hash = trit_array_new(NUM_TRITS_HASH);
  for (int i = 0; i < num_txs; i++) {
    trit_array_set_trits(curr_hash, txs[i].hash, NUM_TRITS_HASH);
    TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_HASH,
                                              curr_hash, &exist) == RC_OK);
    TEST_ASSERT(exist);
  }
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), num_txs);

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) {
    total_weight += curr_cw_entry->value;
  }

  TEST_ASSERT_EQUAL_INT(total_weight, 4 + 2 + 2 + 1);

  ep_randomizer_t ep_randomizer;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(&ep_randomizer, &tangle, 0.01,
                                                EP_RANDOM_WALK) == RC_OK);

  trit_array_t tip;
  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];
  tip.trits = tip_trits;

  init_epv(&epv);
  /// Select the tip
  TEST_ASSERT(iota_consensus_exit_probability_randomize(
                  &ep_randomizer, &epv, &out, ep, &tip) == RC_OK);

  /// Check that tip was selected
  TEST_ASSERT_EQUAL_MEMORY(tip.trits, txs[3].hash, FLEX_TRIT_SIZE_243);

  TEST_ASSERT(iota_consensus_ep_randomizer_destroy(&ep_randomizer) == RC_OK);

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  trit_array_free(ep);
  trit_array_free(curr_hash);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
}

void test_cw_topology_two_inequal_tips(void) {
  hash_to_int_map_entry_t *curr_cw_entry = NULL;
  hash_to_int_map_entry_t *tmp_cw_entry = NULL;

  size_t num_txs = 4;

  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);

  bool exist;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  struct _iota_transaction txs[num_txs];
  txs[0] = TEST_TRANSACTION;
  for (int i = 1; i < num_txs; i++) {
    txs[i] = TEST_TRANSACTION;
    // Different hash for each tx,
    // we don't worry about it not being valid encoding
    txs[i].hash[0] += (i + 1);
  }

  /// First two transactions approve entry point.
  memcpy(txs[1].branch, txs[0].hash, FLEX_TRIT_SIZE_243);
  memcpy(txs[2].branch, txs[0].hash, FLEX_TRIT_SIZE_243);
  /// Third transaction approves second transaction so we now have two tips
  /// (txs[3],txs[2])
  memcpy(txs[3].branch, txs[1].hash, FLEX_TRIT_SIZE_243);
  memcpy(txs[3].trunk, txs[1].hash, FLEX_TRIT_SIZE_243);

  for (int i = 0; i < num_txs; i++) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
  }

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, txs[0].hash, NUM_TRITS_HASH);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, num_txs);

  cw_calc_result out;

  trit_array_p curr_hash = trit_array_new(NUM_TRITS_HASH);
  for (int i = 0; i < num_txs; i++) {
    trit_array_set_trits(curr_hash, txs[i].hash, NUM_TRITS_HASH);
    TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_HASH,
                                              curr_hash, &exist) == RC_OK);
    TEST_ASSERT(exist);
  }
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), num_txs);

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) {
    total_weight += curr_cw_entry->value;
  }

  TEST_ASSERT_EQUAL_INT(total_weight, 4 + 2 + 1 + 1);

  /// Exit Probabilities - start

  ep_randomizer_t ep_randomizer;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(
                  &ep_randomizer, &tangle, low_alpha, EP_RANDOM_WALK) == RC_OK);

  trit_array_t tip;
  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];
  tip.trits = tip_trits;

  init_epv(&epv);
  /// Select the tip

  size_t selected_tip_count = 0;
  int selections = 200;
  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(
                    &ep_randomizer, &epv, &out, ep, &tip) == RC_OK);
    if (memcmp(tip.trits, txs[num_txs - 1].hash, FLEX_TRIT_SIZE_243) == 0) {
      selected_tip_count++;
    }
  }

  // We can look on the previous trial as a sample from
  // binomial distribution where `p` = 1/num_approvers, `n` = selections,
  // so we get (mean = `np`, stdev = `np*(1-p)`):
  double expected_mean = selections / 2;
  double expected_stdev = sqrt(expected_mean * (1 - 1 / 2));
  TEST_ASSERT(selected_tip_count < expected_mean + 3 * expected_stdev);
  TEST_ASSERT(selected_tip_count > expected_mean - 3 * expected_stdev);

  /// High alpha
  ep_randomizer.alpha = high_alpha;
  selected_tip_count = 0;

  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(
                    &ep_randomizer, &epv, &out, ep, &tip) == RC_OK);
    if (memcmp(tip.trits, txs[num_txs - 1].hash, FLEX_TRIT_SIZE_243) == 0) {
      selected_tip_count++;
    }
  }

  TEST_ASSERT_EQUAL_INT(selected_tip_count, selections);

  TEST_ASSERT(iota_consensus_ep_randomizer_destroy(&ep_randomizer) == RC_OK);

  /// Exit Probabilities - end

  /// Exit Probabilities - end

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  trit_array_free(ep);
  trit_array_free(curr_hash);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
}

void test_1_bundle(void) {
  hash_to_int_map_entry_t *curr_cw_entry = NULL;
  hash_to_int_map_entry_t *tmp_cw_entry = NULL;

  size_t bundle_size = 4;

  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);

  bool exist;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  iota_transaction_t txs[bundle_size + 1];

  tryte_t *trytes[5] = {(tryte_t *)TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                        (tryte_t *)TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                        (tryte_t *)TX_3_OF_4_VALUE_BUNDLE_TRYTES,
                        (tryte_t *)TX_4_OF_4_VALUE_BUNDLE_TRYTES,
                        (tryte_t *)BUNDLE_OF_4_TRUNK_TRANSACTION};
  transactions_deserialize(trytes, txs, 5);
  for (size_t i = 0; i < 5; ++i) {
    txs[i]->snapshot_index = 9999999;
  }
  build_tangle(&tangle, txs, bundle_size + 1);

  iota_stor_pack_t pack;
  hash_pack_init(&pack, bundle_size + 1);

  cw_calc_result out;

  for (unsigned int i = 0; i < bundle_size; ++i) {
    TEST_ASSERT(iota_tangle_transaction_load_hashes_of_approvers(
                    &tangle, txs[i]->hash, &pack) == RC_OK);
  }

  TEST_ASSERT(iota_tangle_transaction_load_hashes_of_approvers(
                  &tangle, txs[4]->hash, &pack) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, bundle_size);

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, txs[4]->hash, NUM_TRITS_HASH);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), bundle_size + 1);

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) {
    total_weight += curr_cw_entry->value;
  }

  TEST_ASSERT_EQUAL_INT(total_weight, 5 + 4 + 3 + 2 + 1);

  /// Exit Probabilities - start

  ep_randomizer_t ep_randomizer;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(
                  &ep_randomizer, &tangle, low_alpha, EP_RANDOM_WALK) == RC_OK);

  trit_array_t tip;
  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];
  tip.trits = tip_trits;

  init_epv(&epv);
  /// Select the tip

  DECLARE_PACK_SINGLE_TX(tx, tx_models, tx_pack);

  TEST_ASSERT(iota_tangle_transaction_load(&tangle, TRANSACTION_FIELD_HASH, ep,
                                           &tx_pack) == RC_OK);

  TEST_ASSERT_EQUAL_INT(1, tx_pack.num_loaded);
  size_t selected_tip_count = 0;
  int selections = 100;
  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(
                    &ep_randomizer, &epv, &out, ep, &tip) == RC_OK);
    if (memcmp(tip.trits, txs[0]->hash, FLEX_TRIT_SIZE_243) == 0) {
      selected_tip_count++;
    }
  }

  TEST_ASSERT_EQUAL_INT(selected_tip_count, selections);

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  trit_array_free(ep);

  transactions_free(txs, 5);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
}

void test_2_chained_bundles(void) {
  hash_to_int_map_entry_t *curr_cw_entry = NULL;
  hash_to_int_map_entry_t *tmp_cw_entry = NULL;

  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);

  bool exist;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  iota_transaction_t txs[6];

  tryte_t *trytes[6] = {(tryte_t *)TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                        (tryte_t *)TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                        (tryte_t *)TX_3_OF_4_VALUE_BUNDLE_TRYTES,
                        (tryte_t *)TX_4_OF_4_VALUE_BUNDLE_TRYTES,
                        (tryte_t *)TX_1_OF_2,
                        (tryte_t *)TX_2_OF_2};
  transactions_deserialize(trytes, txs, 6);
  for (size_t i = 0; i < 6; ++i) {
    txs[i]->snapshot_index = 9999999;
  }
  build_tangle(&tangle, txs, 6);

  // First bundle

  flex_trit_t ep_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(ep_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         (tryte_t *)BUNDLE_OF_2_TRUNK_TX,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t txEp = transaction_deserialize(ep_trits);
  txEp->snapshot_index = 9999999;

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, txEp) == RC_OK);

  cw_calc_result out;

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, txEp->hash, NUM_TRITS_HASH);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, &tangle,
                                            DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), 7);

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) {
    total_weight += curr_cw_entry->value;
  }

  TEST_ASSERT_EQUAL_INT(total_weight, 7 + 6 + 5 + 4 + 3 + 2 + 1);

  /// Exit Probabilities - start

  ep_randomizer_t ep_randomizer;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(
                  &ep_randomizer, &tangle, low_alpha, EP_RANDOM_WALK) == RC_OK);

  trit_array_t tip;
  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];
  tip.trits = tip_trits;

  init_epv(&epv);
  /// Select the tip

  DECLARE_PACK_SINGLE_TX(tx, tx_models, tx_pack);

  TEST_ASSERT(iota_tangle_transaction_load(&tangle, TRANSACTION_FIELD_HASH, ep,
                                           &tx_pack) == RC_OK);

  TEST_ASSERT_EQUAL_INT(1, tx_pack.num_loaded);
  size_t selected_tip_count = 0;
  int selections = 10;
  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(
                    &ep_randomizer, &epv, &out, ep, &tip) == RC_OK);
    if (memcmp(tip.trits, txs[0]->hash, FLEX_TRIT_SIZE_243) == 0) {
      selected_tip_count++;
    }
  }

  TEST_ASSERT_EQUAL_INT(selected_tip_count, selections);

  cw_calc_result_destroy(&out);
  trit_array_free(ep);
  transactions_free(txs, 6);
  transaction_free(txEp);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
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
  }

  config.db_path = test_db_path;

  RUN_TEST(test_single_tx_tangle);
  RUN_TEST(test_cw_topology_blockchain);
  RUN_TEST(test_cw_topology_only_direct_approvers);
  RUN_TEST(test_cw_topology_four_transactions_diamond);
  RUN_TEST(test_cw_topology_two_inequal_tips);

  // Bundles
  RUN_TEST(test_1_bundle);
  RUN_TEST(test_2_chained_bundles);

  return UNITY_END();
}
