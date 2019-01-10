/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __PERCEPTIVE_NODE_PERCEPTIVE_NODE_H__
#define __PERCEPTIVE_NODE_PERCEPTIVE_NODE_H__

#include <stdbool.h>
#include "ciri/perceptive_node/conf.h"
#include "ciri/perceptive_node/neighbor_t_to_uint32_t_map.h"
#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "consensus/consensus.h"
#include "gossip/neighbor.h"
#include "utils/containers/arrays/double_array.h"
#include "utils/containers/hash/hash_array.h"
#include "utils/containers/hash/hash_hash_pair_t_map.h"

#include "utils/handles/thread.h"
#include "utils/logger_helper.h"

#include "utarray.h"

/**
 * The IOTA Perceptive node implementation
 */

typedef UT_array transactions_sequence_t;

typedef struct perceptive_node_monitoring_data_s {
  hash_array_p monitored_transactions_seq;
  neighbor_t *monitored_neighbor;
  // Current monitoring session starting time
  uint64_t monitoring_start_timestamp;
  // Next monitoring session starting time
  uint64_t monitoring_next_timestamp;
  // The entire subtangle (from an entry point) represented as a map between
  // a transaction and it's direct approvers, this is the result of
  // "iota_consensus_cw_rating_calculate"
  // and is used for exit probability calculation
  hash_to_indexed_hash_set_map_t *base_approvers_map;
  uint64_t subtangle_size;
  // A set containing hashes of transactions that were received from
  // the currently monitored neighbor, since current monitoring session has
  // started
  hash_to_hash_pair_t_map_t txs_from_monitored_neighbor;
  uint32_t txs_from_monitored_neighbor_size;
  // A vector containing all hashes in the order they were received
  // From all neighbors, since current monitoring session has started.
  transactions_sequence_t *txs_sequence_from_all_neighbors;
  bool is_currently_monitoring;
  flex_trit_t entry_point[FLEX_TRIT_SIZE_243];
  // Should contain lf scores of sequences that were generated using tip
  // selection
  double_array_p test_lf_distribution_samples;
} perceptive_node_monitoring_data_t;

typedef struct iota_perceptive_node_s {
  iota_perceptive_node_conf_t conf;
  thread_handle_t thread;
  // TODO - atomicity
  bool test_thread_running;
  tangle_t *tangle;
  iota_consensus_t *consensus;
  cw_rating_calculator_t cw_calc;
  neighbor_t *neighbors;
  neighbor_t_to_uint32_t_map_t neighbors_to_recent_transactions_count;
  perceptive_node_monitoring_data_t monitoring_data;
} iota_perceptive_node_t;

#ifdef __cplusplus
extern "C" {
/**
 * Initializes a Perceptive Node
 *
 * @param pn The Perceptive Node
 * @param consensus A consensus
 *
 * @return a status code
 */
retcode_t iota_perceptive_node_init(iota_perceptive_node_t *const pn,
                                    iota_consensus_t const *const consensus,
                                    neighbor_t const *const neighbors);

/**
 * Starts a Perceptive Node
 *
 * @param pn The Perceptive Node
 *
 * @return a status code
 */
retcode_t iota_perceptive_node_start(iota_perceptive_node_t *const pn);

/**
 * Stops a Perceptive Node
 *
 * @param pn The Perceptive Node
 *
 * @return a status code
 */
retcode_t iota_perceptive_node_stop(iota_perceptive_node_t *const pn);

/**
 * Destroys a Perceptive Node
 *
 * @param pn The Perceptive Node
 *
 * @return a status code
 */
retcode_t iota_perceptive_node_destroy(iota_perceptive_node_t *const pn);

/**
 * Notifies about the arrival of new transaction
 *
 * @param pn The Perceptive Node
 * @param transaction The Transaction's that was gossiped
 * @param from The neighbor that sent this transaction
 *
 * @return a status code
 */
retcode_t iota_perceptive_node_on_next_transaction(
    iota_perceptive_node_t *const pn,
    iota_transaction_t const *const transaction, neighbor_t *const from);
};
#endif

#endif  //__PERCEPTIVE_NODE_PERCEPTIVE_NODE_H__
