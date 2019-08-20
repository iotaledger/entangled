/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "utarray.h"

#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "ciri/consensus/tip_selection/cw_rating_calculator/cw_rating_calculator.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/exit_prob_map.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/exit_probability_randomizer.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/walker.h"
#include "ciri/consensus/transaction_solidifier/transaction_solidifier.h"
#include "common/model/transaction.h"
#include "common/storage/connection.h"
#include "common/storage/storage.h"
#include "common/storage/tests/defs.h"
#include "utils/containers/hash/hash_double_map.h"
#include "utils/macros.h"
#include "utils/time.h"

static cw_rating_calculator_t calc;
static tangle_t tangle;
static storage_connection_config_t config;
static iota_consensus_conf_t conf;

static char *tangle_test_db_path = "ciri/consensus/tip_selection/exit_probability_randomizer/tests/test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static char *snapshot_path = "ciri/consensus/tip_selection/exit_probability_randomizer/tests/snapshot.txt";
static char *snapshot_conf_path = "ciri/consensus/snapshot/tests/snapshot_conf.json";

static double low_alpha = 0;
static double high_alpha = 10;

static uint32_t max_txs_below_max_depth = 10000;
static uint32_t max_depth = 15;

typedef enum test_tangle_topology {
  ONLY_DIRECT_APPROVERS,
  BLOCKCHAIN,
} test_tangle_topology;

static exit_prob_transaction_validator_t epv;
static snapshots_provider_t snapshots_provider;
static milestone_tracker_t mt;
static ledger_validator_t lv;
static transaction_solidifier_t ts;

void test_sum_probabilities_1_ep_mapping(ep_randomizer_t *const ep_randomizer, flex_trit_t const *const ep,
                                         cw_calc_result const *const out);
void test_1_bundle(ep_randomizer_implementation_t ep_impl, ep_randomizer_t *const ep_randomizer);
void test_cw_topology_five_transactions_diamond_and_a_tail(ep_randomizer_implementation_t ep_impl,
                                                           ep_randomizer_t *const ep_randomizer);
void test_cw_topology_two_inequal_tips(ep_randomizer_implementation_t ep_impl, ep_randomizer_t *const ep_randomizer);
void test_2_chained_bundles(ep_randomizer_implementation_t ep_impl, ep_randomizer_t *const ep_randomizer);
void test_cw_topology_four_transactions_diamond(ep_randomizer_implementation_t ep_impl,
                                                ep_randomizer_t *const ep_randomizer);
void test_single_tx_tangle_base(ep_randomizer_implementation_t ep_impl, ep_randomizer_t *const ep_randomizer);

void setUp() { TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK); }

void tearDown() { TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK); }

static void init_epv(exit_prob_transaction_validator_t *const epv) {
  conf.max_depth = max_depth;
  conf.below_max_depth = max_txs_below_max_depth;

  strcpy(conf.snapshot_file, snapshot_path);
  strcpy(conf.snapshot_conf_file, snapshot_conf_path);
  conf.snapshot_signature_skip_validation = true;

  // Avoid complete initialization with state file loading
  TEST_ASSERT(iota_snapshot_reset(&snapshots_provider.initial_snapshot, &conf) == RC_OK);
  TEST_ASSERT(iota_snapshot_reset(&snapshots_provider.latest_snapshot, &conf) == RC_OK);
  TEST_ASSERT(iota_consensus_transaction_solidifier_init(&ts, &conf, NULL, &snapshots_provider, NULL) == RC_OK);
  TEST_ASSERT(iota_milestone_tracker_init(&mt, &conf, &snapshots_provider, &lv, &ts) == RC_OK);
  TEST_ASSERT(iota_consensus_ledger_validator_init(&lv, &tangle, &conf, &mt) == RC_OK);

  // We want to avoid unnecessary validation
  mt.snapshots_provider->latest_snapshot.metadata.index = 9999999;
  mt.latest_solid_milestone_index = max_depth;

  TEST_ASSERT(iota_consensus_exit_prob_transaction_validator_init(&conf, &mt, &lv, epv) == RC_OK);
}

static void destroy_epv(exit_prob_transaction_validator_t *epv) {
  iota_consensus_ledger_validator_destroy(epv->lv);
  iota_milestone_tracker_destroy(&mt);
  iota_consensus_exit_prob_transaction_validator_destroy(epv);
  iota_consensus_transaction_solidifier_destroy(&ts);
  iota_snapshots_provider_destroy(&snapshots_provider);
}

void test_cw_gen_topology(test_tangle_topology topology, ep_randomizer_implementation_t ep_impl,
                          ep_randomizer_t *const ep_randomizer) {
  hash_to_int64_t_map_entry_t *curr_cw_entry = NULL;
  hash_to_int64_t_map_entry_t *tmp_cw_entry = NULL;
  size_t num_approvers = 50;
  size_t num_txs = num_approvers + 1;

  init_epv(&epv);

  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx = transaction_deserialize(tx_trits, true);
  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, tx->consensus.hash, true) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, tx->consensus.hash, max_depth) == RC_OK);

  iota_transaction_t txs[num_approvers];
  txs[0] = *tx;
  txs[0].consensus.hash[0] += 1;
  transaction_set_branch(&txs[0], transaction_hash(tx));
  for (size_t i = 1; i < num_approvers; i++) {
    txs[i] = *tx;
    // Different hash for each tx,
    // we don't worry about it not being valid encoding
    txs[i].consensus.hash[i / 256] += (i + 1);
    if (topology == ONLY_DIRECT_APPROVERS) {
      transaction_set_branch(&txs[i], transaction_branch(&txs[i - 1]));
      transaction_set_trunk(&txs[i], transaction_trunk(&txs[i - 1]));
    } else if (topology == BLOCKCHAIN) {
      transaction_set_branch(&txs[i], transaction_hash(&txs[i - 1]));
      transaction_set_trunk(&txs[i], transaction_hash(&txs[i - 1]));
    }
  }

  for (size_t i = 0; i < num_approvers; i++) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, txs[i].consensus.hash, true) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, txs[i].consensus.hash, max_depth) == RC_OK);
  }

  flex_trit_t *ep = transaction_hash(tx);

  cw_calc_result out;
  bool exist;
  for (size_t i = 0; i < num_approvers; i++) {
    TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_HASH, transaction_hash(&txs[i]), &exist) ==
                RC_OK);
    TEST_ASSERT(exist);
  }
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, &tangle, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(num_approvers + 1, HASH_COUNT(out.tx_to_approvers));

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) { total_weight += curr_cw_entry->value; }
  // First Entry, for both topologies, has to equal the number of total
  // approvers + own weight
  TEST_ASSERT_EQUAL_INT64(out.cw_ratings->value, num_approvers + 1);

  if (topology == ONLY_DIRECT_APPROVERS) {
    TEST_ASSERT_EQUAL_INT64(total_weight, num_approvers + 1 + num_approvers);
  } else if (topology == BLOCKCHAIN) {
    // Sum of series 1 + 2 + ... + (num_txs)
    TEST_ASSERT_EQUAL_INT64(total_weight, (num_txs * (num_txs + 1)) / 2);
  }

  /// Exit Probabilities - start

  conf.alpha = 0;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(ep_randomizer, &conf, ep_impl) == RC_OK);

  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];

  /// Select the tip
  uint16_t selected_tip_counts[num_approvers];
  memset(selected_tip_counts, 0, sizeof(selected_tip_counts));

  size_t selections = 200;
  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(ep_randomizer, &tangle, &epv, &out, ep, tip_trits) == RC_OK);

    for (size_t a = 0; a < num_approvers; ++a) {
      if (memcmp(tip_trits, transaction_hash(&txs[a]), FLEX_TRIT_SIZE_243) == 0) {
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
    double expected_stdev = sqrt(expected_mean * (1.0 - 1.0 / ((double)num_approvers)));
    for (size_t a = 0; a < num_approvers; ++a) {
      uint16_t comp_up = expected_mean + 4 * expected_stdev;
      uint16_t comp_low = MAX((expected_mean - 4 * expected_stdev), 0);
      TEST_ASSERT(selected_tip_counts[a] <= comp_up);
      TEST_ASSERT(selected_tip_counts[a] >= comp_low);
    }
  }
  test_sum_probabilities_1_ep_mapping(ep_randomizer, ep, &out);

  for (size_t a = 0; a < num_approvers; ++a) {
    total_selections += selected_tip_counts[a];
  }
  TEST_ASSERT(total_selections == selections);

  /// Exit Probabilities - end
  cw_calc_result_destroy(&out);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
  transaction_free(tx);
}

void test_single_tx_tangle_walker(void) {
  ep_randomizer_t ep_randomizer;
  test_single_tx_tangle_base(EP_RANDOM_WALK, &ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy(&ep_randomizer) == RC_OK);
}

void test_single_tx_tangle_map(void) {
  ep_prob_map_randomizer_t ep_randomizer;
  test_single_tx_tangle_base(EP_RANDOMIZE_MAP_AND_SAMPLE, (ep_randomizer_t *)&ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *)&ep_randomizer) == RC_OK);
}

void test_single_tx_tangle_base(ep_randomizer_implementation_t ep_impl, ep_randomizer_t *const ep_randomizer) {
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, DFS_FROM_ENTRY_POINT) == RC_OK);
  init_epv(&epv);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, 5);

  cw_calc_result out;
  bool exist = false;

  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx = transaction_deserialize(tx_trits, true);

  flex_trit_t *ep = transaction_hash(tx);

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, tx->consensus.hash, true) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, tx->consensus.hash, max_depth) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(exist == true);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, &tangle, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), 1);

  conf.alpha = 0.01;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(ep_randomizer, &conf, ep_impl) == RC_OK);

  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];

  /// Select the tip
  TEST_ASSERT(iota_consensus_exit_probability_randomize(ep_randomizer, &tangle, &epv, &out, ep, tip_trits) == RC_OK);

  /// Check that tip was selected

  TEST_ASSERT_EQUAL_MEMORY(tip_trits, ep, FLEX_TRIT_SIZE_243);
  test_sum_probabilities_1_ep_mapping(ep_randomizer, ep, &out);

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
  transaction_free(tx);
}
void test_cw_topology_only_direct_approvers_walker(void) {
  ep_randomizer_t ep_randomizer;
  test_cw_gen_topology(ONLY_DIRECT_APPROVERS, EP_RANDOM_WALK, &ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy(&ep_randomizer) == RC_OK);
}

void test_cw_topology_only_direct_approvers_map(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_cw_gen_topology(ONLY_DIRECT_APPROVERS, EP_RANDOMIZE_MAP_AND_SAMPLE, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_cw_topology_blockchain_walker(void) {
  ep_randomizer_t ep_randomizer = {};
  test_cw_gen_topology(BLOCKCHAIN, EP_RANDOM_WALK, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}
void test_cw_topology_blockchain_map(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_cw_gen_topology(BLOCKCHAIN, EP_RANDOMIZE_MAP_AND_SAMPLE, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_cw_topology_four_transactions_diamond_walker(void) {
  ep_randomizer_t ep_randomizer = {};
  test_cw_topology_four_transactions_diamond(EP_RANDOM_WALK, &ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy(&ep_randomizer) == RC_OK);
}

void test_cw_topology_four_transactions_diamond_map(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_cw_topology_four_transactions_diamond(EP_RANDOMIZE_MAP_AND_SAMPLE, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_cw_topology_four_transactions_diamond(ep_randomizer_implementation_t ep_impl,
                                                ep_randomizer_t *const ep_randomizer) {
  hash_to_int64_t_map_entry_t *curr_cw_entry = NULL;
  hash_to_int64_t_map_entry_t *tmp_cw_entry = NULL;

  size_t num_txs = 4;

  bool exist;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  init_epv(&epv);

  flex_trit_t test_tx_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(test_tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *test_tx = transaction_deserialize(test_tx_trits, true);

  iota_transaction_t txs[num_txs];
  txs[0] = *test_tx;
  for (size_t i = 1; i < num_txs; i++) {
    txs[i] = *test_tx;
    // Different hash for each tx,
    // we don't worry about it not being valid encoding
    txs[i].consensus.hash[0] += (i + 1);
  }

  /// First two transactions approve entry point.
  transaction_set_branch(&txs[1], transaction_hash(&txs[0]));
  transaction_set_branch(&txs[2], transaction_hash(&txs[0]));
  /// Third transaction approves first two via branch+trunk
  transaction_set_branch(&txs[3], transaction_hash(&txs[1]));
  transaction_set_trunk(&txs[3], transaction_hash(&txs[2]));

  for (size_t i = 0; i < num_txs; i++) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, txs[i].consensus.hash, true) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, txs[i].consensus.hash, max_depth) == RC_OK);
  }

  flex_trit_t *ep = transaction_hash(&txs[0]);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, num_txs);

  cw_calc_result out;

  for (size_t i = 0; i < num_txs; i++) {
    TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_HASH, transaction_hash(&txs[i]), &exist) ==
                RC_OK);
    TEST_ASSERT(exist);
  }
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, &tangle, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), num_txs);

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) { total_weight += curr_cw_entry->value; }

  TEST_ASSERT_EQUAL_INT(total_weight, 4 + 2 + 2 + 1);

  conf.alpha = 0.01;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(ep_randomizer, &conf, ep_impl) == RC_OK);

  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];

  /// Select the tip
  TEST_ASSERT(iota_consensus_exit_probability_randomize(ep_randomizer, &tangle, &epv, &out, ep, tip_trits) == RC_OK);

  /// Check that tip was selected
  TEST_ASSERT_EQUAL_MEMORY(tip_trits, transaction_hash(&txs[3]), FLEX_TRIT_SIZE_243);

  test_sum_probabilities_1_ep_mapping(ep_randomizer, ep, &out);

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
  transaction_free(test_tx);
}

/*
 * This topology is useful to check that we do not
 * over-sum transition probabilities in when re-visiting
 * same edge in "iota_consensus_random_walker_map_eps"
 */

void test_cw_topology_five_transactions_diamond_and_a_tail_walker(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_cw_topology_five_transactions_diamond_and_a_tail(EP_RANDOM_WALK, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_cw_topology_five_transactions_diamond_and_a_tail_map(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_cw_topology_five_transactions_diamond_and_a_tail(EP_RANDOMIZE_MAP_AND_SAMPLE,
                                                        (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_cw_topology_five_transactions_diamond_and_a_tail(ep_randomizer_implementation_t ep_impl,
                                                           ep_randomizer_t *const ep_randomizer) {
  hash_to_int64_t_map_entry_t *curr_cw_entry = NULL;
  hash_to_int64_t_map_entry_t *tmp_cw_entry = NULL;

  size_t num_txs = 5;

  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, DFS_FROM_ENTRY_POINT) == RC_OK);

  bool exist;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  init_epv(&epv);

  flex_trit_t test_tx_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(test_tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *test_tx = transaction_deserialize(test_tx_trits, true);

  iota_transaction_t txs[num_txs];
  txs[0] = *test_tx;

  for (size_t i = 1; i < num_txs; i++) {
    txs[i] = *test_tx;
    // Different hash for each tx,
    // we don't worry about it not being valid encoding
    txs[i].consensus.hash[0] += (i + 1);
  }

  /// First two transactions approve entry point.
  transaction_set_branch(&txs[1], transaction_hash(&txs[0]));
  transaction_set_branch(&txs[2], transaction_hash(&txs[0]));
  /// Third transaction approves first two via branch+trunk
  transaction_set_branch(&txs[3], transaction_hash(&txs[1]));
  transaction_set_trunk(&txs[3], transaction_hash(&txs[2]));
  /// Fourth transaction approves the third
  transaction_set_branch(&txs[4], transaction_hash(&txs[3]));
  transaction_set_trunk(&txs[4], transaction_hash(&txs[3]));

  for (size_t i = 0; i < num_txs; i++) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, txs[i].consensus.hash, true) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, txs[i].consensus.hash, max_depth) == RC_OK);
  }

  flex_trit_t *ep = transaction_hash(&txs[0]);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, num_txs);

  cw_calc_result out;

  for (size_t i = 0; i < num_txs; i++) {
    TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_HASH, transaction_hash(&txs[i]), &exist) ==
                RC_OK);
    TEST_ASSERT(exist);
  }
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, &tangle, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), num_txs);

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) { total_weight += curr_cw_entry->value; }

  TEST_ASSERT_EQUAL_INT(total_weight, 5 + 3 + 3 + 2 + 1);

  conf.alpha = 0.01;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(ep_randomizer, &conf, ep_impl) == RC_OK);

  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];

  /// Select the tip
  TEST_ASSERT(iota_consensus_exit_probability_randomize(ep_randomizer, &tangle, &epv, &out, ep, tip_trits) == RC_OK);

  /// Check that tip was selected
  TEST_ASSERT_EQUAL_MEMORY(tip_trits, transaction_hash(&txs[num_txs - 1]), FLEX_TRIT_SIZE_243);

  /// Extract Tips

  test_sum_probabilities_1_ep_mapping(ep_randomizer, ep, &out);

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
  transaction_free(test_tx);
}

void test_cw_topology_two_inequal_tips_walker(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_cw_topology_two_inequal_tips(EP_RANDOM_WALK, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_cw_topology_two_inequal_tips_map(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_cw_topology_two_inequal_tips(EP_RANDOMIZE_MAP_AND_SAMPLE, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}
void test_cw_topology_two_inequal_tips(ep_randomizer_implementation_t ep_impl, ep_randomizer_t *const ep_randomizer) {
  hash_to_int64_t_map_entry_t *curr_cw_entry = NULL;
  hash_to_int64_t_map_entry_t *tmp_cw_entry = NULL;

  size_t num_txs = 4;

  init_epv(&epv);
  bool exist;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES, NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *test_tx = transaction_deserialize(tx_trits, true);

  iota_transaction_t txs[num_txs];
  txs[0] = *test_tx;
  for (size_t i = 1; i < num_txs; i++) {
    txs[i] = *test_tx;
    // Different hash for each tx,
    // we don't worry about it not being valid encoding
    txs[i].consensus.hash[0] += (i + 1);
  }

  /// First two transactions approve entry point.
  transaction_set_branch(&txs[1], transaction_hash(&txs[0]));
  transaction_set_branch(&txs[2], transaction_hash(&txs[0]));
  /// Third transaction approves first two via branch+trunk
  transaction_set_branch(&txs[3], transaction_hash(&txs[1]));
  transaction_set_trunk(&txs[3], transaction_hash(&txs[1]));

  for (size_t i = 0; i < num_txs; i++) {
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, txs[i].consensus.hash, true) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, txs[i].consensus.hash, max_depth) == RC_OK);
  }

  flex_trit_t *ep = transaction_hash(&txs[0]);
  iota_stor_pack_t pack;
  hash_pack_init(&pack, num_txs);

  cw_calc_result out;

  for (size_t i = 0; i < num_txs; i++) {
    TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_HASH, transaction_hash(&txs[i]), &exist) ==
                RC_OK);
    TEST_ASSERT(exist);
  }
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, &tangle, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), num_txs);

  int total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) { total_weight += curr_cw_entry->value; }

  TEST_ASSERT_EQUAL_INT(total_weight, 4 + 2 + 1 + 1);

  /// Exit Probabilities - start
  conf.alpha = low_alpha;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(ep_randomizer, &conf, ep_impl) == RC_OK);

  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];

  /// Select the tip

  size_t selected_tip_count = 0;
  size_t selections = 200;
  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(ep_randomizer, &tangle, &epv, &out, ep, tip_trits) == RC_OK);
    if (memcmp(tip_trits, transaction_hash(&txs[num_txs - 1]), FLEX_TRIT_SIZE_243) == 0) {
      selected_tip_count++;
    }
  }

  // We can look on the previous trial as a sample from
  // binomial distribution where `p` = 1/num_approvers, `n` = selections,
  // so we get (mean = `np`, stdev = `np*(1-p)`):
  double expected_mean = ((double)selections) / 2;
  double expected_stdev = sqrt(expected_mean * (1 - 1 / 2));
  TEST_ASSERT(selected_tip_count < expected_mean + 3 * expected_stdev);
  TEST_ASSERT(selected_tip_count > expected_mean - 3 * expected_stdev);

  /// High alpha
  conf.alpha = high_alpha;
  selected_tip_count = 0;
  // When setting new alpha need to reset probs
  if (ep_impl == EP_RANDOMIZE_MAP_AND_SAMPLE) {
    iota_consensus_exit_prob_map_reset(ep_randomizer);
  }

  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(ep_randomizer, &tangle, &epv, &out, ep, tip_trits) == RC_OK);
    if (memcmp(tip_trits, transaction_hash(&txs[num_txs - 1]), FLEX_TRIT_SIZE_243) == 0) {
      selected_tip_count++;
    }
  }

  TEST_ASSERT_EQUAL_INT(selected_tip_count, selections);

  test_sum_probabilities_1_ep_mapping(ep_randomizer, ep, &out);

  /// Exit Probabilities - end

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
  transaction_free(test_tx);
}

void test_1_bundle_walker(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_1_bundle(EP_RANDOM_WALK, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_1_bundle_map(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_1_bundle(EP_RANDOMIZE_MAP_AND_SAMPLE, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_1_bundle(ep_randomizer_implementation_t ep_impl, ep_randomizer_t *const ep_randomizer) {
  hash_to_int64_t_map_entry_t *curr_cw_entry = NULL;
  hash_to_int64_t_map_entry_t *tmp_cw_entry = NULL;

  size_t bundle_size = 4;
  init_epv(&epv);

  bool exist;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  iota_transaction_t *txs[bundle_size + 1];

  tryte_t const *const trytes[5] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES,
                                    BUNDLE_OF_4_TRUNK_TRANSACTION};
  transactions_deserialize(trytes, txs, 5, true);

  transaction_set_trunk(txs[4], conf.genesis_hash);
  transaction_set_branch(txs[4], conf.genesis_hash);
  build_tangle(&tangle, txs, bundle_size + 1);

  iota_stor_pack_t pack;
  hash_pack_init(&pack, bundle_size + 1);

  cw_calc_result out;

  for (unsigned int i = 0; i < bundle_size; ++i) {
    TEST_ASSERT(iota_tangle_transaction_load_hashes_of_approvers(&tangle, transaction_hash(txs[i]), &pack, 0) == RC_OK);
  }

  for (size_t i = 0; i < 5; ++i) {
    TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, txs[i]->consensus.hash, true) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, txs[i]->consensus.hash, max_depth) == RC_OK);
  }

  TEST_ASSERT(iota_tangle_transaction_load_hashes_of_approvers(&tangle, transaction_hash(txs[4]), &pack,
                                                               current_timestamp_ms()) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, bundle_size);
  hash_pack_reset(&pack);
  TEST_ASSERT(iota_tangle_transaction_load_hashes_of_approvers(&tangle, transaction_hash(txs[4]), &pack,
                                                               current_timestamp_ms() / 2) == RC_OK);
  TEST_ASSERT_EQUAL_INT(pack.num_loaded, 0);

  flex_trit_t *ep = transaction_hash(txs[4]);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, &tangle, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), bundle_size + 1);

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) { total_weight += curr_cw_entry->value; }

  TEST_ASSERT_EQUAL_INT(total_weight, 5 + 4 + 3 + 2 + 1);

  /// Exit Probabilities - start

  conf.alpha = low_alpha;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(ep_randomizer, &conf, ep_impl) == RC_OK);
  test_sum_probabilities_1_ep_mapping(ep_randomizer, ep, &out);

  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];

  /// Select the tip

  size_t selected_tip_count = 0;
  size_t selections = 100;
  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(ep_randomizer, &tangle, &epv, &out, ep, tip_trits) == RC_OK);
    if (memcmp(tip_trits, transaction_hash(txs[0]), FLEX_TRIT_SIZE_243) == 0) {
      selected_tip_count++;
    }
  }

  TEST_ASSERT_EQUAL_INT(selected_tip_count, selections);

  hash_pack_free(&pack);
  cw_calc_result_destroy(&out);

  transactions_free(txs, 5);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
}

void test_2_chained_bundles_walker(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_2_chained_bundles(EP_RANDOM_WALK, (ep_randomizer_t *const) & ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_2_chained_bundles_map(void) {
  ep_prob_map_randomizer_t ep_randomizer = {};
  test_2_chained_bundles(EP_RANDOMIZE_MAP_AND_SAMPLE, (ep_randomizer_t *)&ep_randomizer);
  TEST_ASSERT(iota_consensus_ep_randomizer_destroy((ep_randomizer_t *const) & ep_randomizer) == RC_OK);
}

void test_2_chained_bundles(ep_randomizer_implementation_t ep_impl, ep_randomizer_t *const ep_randomizer) {
  hash_to_int64_t_map_entry_t *curr_cw_entry = NULL;
  hash_to_int64_t_map_entry_t *tmp_cw_entry = NULL;

  bool exist;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);

  TEST_ASSERT(!exist);

  iota_transaction_t *txs[6];

  tryte_t const *const trytes[6] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_3_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_4_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_1_OF_2,
                                    TX_2_OF_2};
  transactions_deserialize(trytes, txs, 6, true);
  build_tangle(&tangle, txs, 6);
  for (size_t i = 0; i < 6; ++i) {
    TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, txs[i]->consensus.hash, true) == RC_OK);
    TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, txs[i]->consensus.hash, max_depth) == RC_OK);
  }

  // First bundle

  flex_trit_t ep_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(ep_trits, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t *)BUNDLE_OF_2_TRUNK_TX,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx_entry_point = transaction_deserialize(ep_trits, true);
  transaction_set_trunk(tx_entry_point, conf.genesis_hash);
  transaction_set_branch(tx_entry_point, conf.genesis_hash);
  transaction_set_snapshot_index(tx_entry_point, 9999999);
  transaction_set_solid(tx_entry_point, true);

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx_entry_point) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, tx_entry_point->consensus.hash, true) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, tx_entry_point->consensus.hash, max_depth) ==
              RC_OK);

  cw_calc_result out;

  flex_trit_t *ep = transaction_hash(tx_entry_point);
  TEST_ASSERT(iota_consensus_cw_rating_init(&calc, DFS_FROM_ENTRY_POINT) == RC_OK);
  TEST_ASSERT(iota_consensus_cw_rating_calculate(&calc, &tangle, ep, &out) == RC_OK);
  TEST_ASSERT_EQUAL_INT(HASH_COUNT(out.tx_to_approvers), 7);

  size_t total_weight = 0;

  HASH_ITER(hh, out.cw_ratings, curr_cw_entry, tmp_cw_entry) { total_weight += curr_cw_entry->value; }

  TEST_ASSERT_EQUAL_INT(total_weight, 7 + 6 + 5 + 4 + 3 + 2 + 1);

  /// Exit Probabilities - start
  conf.alpha = low_alpha;
  TEST_ASSERT(iota_consensus_ep_randomizer_init(ep_randomizer, &conf, ep_impl) == RC_OK);

  flex_trit_t tip_trits[FLEX_TRIT_SIZE_243];

  init_epv(&epv);
  /// Select the tip
  size_t selected_tip_count = 0;
  size_t selections = 10;
  for (size_t i = 0; i < selections; ++i) {
    TEST_ASSERT(iota_consensus_exit_probability_randomize(ep_randomizer, &tangle, &epv, &out, ep, tip_trits) == RC_OK);
    if (memcmp(tip_trits, transaction_hash(txs[0]), FLEX_TRIT_SIZE_243) == 0) {
      selected_tip_count++;
    }
  }

  TEST_ASSERT_EQUAL_INT(selected_tip_count, selections);

  cw_calc_result_destroy(&out);
  transactions_free(txs, 6);
  transaction_free(tx_entry_point);
  TEST_ASSERT(iota_consensus_cw_rating_destroy(&calc) == RC_OK);
  destroy_epv(&epv);
}

void test_sum_probabilities_1_ep_mapping(ep_randomizer_t *const ep_randomizer, flex_trit_t const *const ep,
                                         cw_calc_result const *const out) {
  hash_to_double_map_t hash_to_exit_probs = NULL;
  hash_to_double_map_t hash_to_trans_probs = NULL;

  iota_consensus_exit_prob_map_calculate_probs(ep_randomizer, &tangle, &epv, (cw_calc_result *)out, ep,
                                               &hash_to_exit_probs, &hash_to_trans_probs);
  double sum_exit_probs = iota_consensus_exit_prob_map_sum_probs(&hash_to_exit_probs);
  double sum_trans_probs = iota_consensus_exit_prob_map_sum_probs(&hash_to_trans_probs);

  TEST_ASSERT_EQUAL_INT(sum_exit_probs, 1);
  TEST_ASSERT(sum_trans_probs >= 1);
  hash_to_double_map_free(&hash_to_exit_probs);
  hash_to_double_map_free(&hash_to_trans_probs);
}

int main() {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;

  iota_consensus_conf_init(&conf);

  RUN_TEST(test_single_tx_tangle_walker);
  RUN_TEST(test_single_tx_tangle_map);
  RUN_TEST(test_cw_topology_blockchain_walker);
  RUN_TEST(test_cw_topology_blockchain_map);
  RUN_TEST(test_cw_topology_only_direct_approvers_walker);
  RUN_TEST(test_cw_topology_only_direct_approvers_map);
  RUN_TEST(test_cw_topology_four_transactions_diamond_walker);
  RUN_TEST(test_cw_topology_four_transactions_diamond_map);
  RUN_TEST(test_cw_topology_two_inequal_tips_walker);
  RUN_TEST(test_cw_topology_two_inequal_tips_map);
  RUN_TEST(test_cw_topology_five_transactions_diamond_and_a_tail_walker);
  RUN_TEST(test_cw_topology_five_transactions_diamond_and_a_tail_map);

  // Bundles
  RUN_TEST(test_1_bundle_walker);
  RUN_TEST(test_1_bundle_map);
  RUN_TEST(test_2_chained_bundles_walker);
  RUN_TEST(test_2_chained_bundles_map);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
