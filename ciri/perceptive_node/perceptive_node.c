/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/perceptive_node/perceptive_node.h"
#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"

#define PERCEPTIVE_NODE_LOGGER_ID "perceptive_node"

/*
 * Private functions
 */

static void clear_monitoring_data(iota_perceptive_node_t *const pn) {
  hash243_set_free(&pn->monitoring_data.txs_from_monitored_neighbor);
  utarray_clear(pn->monitoring_data.txs_sequence_from_all_neighbors);
  hash_to_indexed_hash_set_map_free(pn->monitoring_data.base_approvers_map);
}

static void destroy_monitoring_data(iota_perceptive_node_t *const pn) {
  hash243_set_free(&pn->monitoring_data.txs_from_monitored_neighbor);
  hash_array_free(pn->monitoring_data.txs_sequence_from_all_neighbors);
  hash_to_indexed_hash_set_map_free(pn->monitoring_data.base_approvers_map);
}

static void *perceptive_node_do_test(iota_perceptive_node_t *const pn) {
  pn->test_thread_running = true;
  // TODO -
  // Pick a neighbor and start monitoring
  log_info(PERCEPTIVE_NODE_LOGGER_ID, "In %s, running likelihood test\n",
           __FUNCTION__);
  if (cw_rating_dfs_do_dfs_from_db(
          &pn->cw_calc, pn->tangle, pn->monitoring_data.entry_point,
          pn->monitoring_data.base_approvers_map,
          &pn->monitoring_data.subtangle_size,
          pn->monitoring_data.monitoring_start_timestamp) != RC_OK) {
    log_error(PERCEPTIVE_NODE_LOGGER_ID,
              "In %s, failed in ratings calculation\n", __FUNCTION__);
    goto cleanup;
  }

  // TODO -
  // clone base map
  // calculate LF
  // simulate dist of same size sequences
  // calculate score of observed LF in comparison to simulated dist and log it
  // Log test duration

cleanup:
  clear_monitoring_data(pn);
  return NULL;
}

retcode_t iota_perceptive_node_init(struct iota_perceptive_node_s *const pn,
                                    iota_consensus_t const *const consensus,
                                    neighbor_t const *const neighbors) {
  retcode_t ret = RC_OK;
  if (pn == NULL) {
    return RC_NULL_PARAM;
  }

  pn->neighbors = NULL;
  logger_helper_init(PERCEPTIVE_NODE_LOGGER_ID, LOGGER_DEBUG, true);
  pn->test_thread_running = false;
  pn->consensus = consensus;
  pn->monitoring_data.base_approvers_map = NULL;

  connection_config_t db_conf = {.db_path = pn->conf.db_path};
  if ((ret = iota_tangle_init(&pn->tangle, &db_conf)) != RC_OK) {
    log_critical(PERCEPTIVE_NODE_LOGGER_ID,
                 "Initializing tangle connection failed\n");
    return ret;
  }

  hash243_array_new(&pn->monitoring_data.monitored_transactions_seq);
  // TODO - conf
  hash_array_reserve(&pn->monitoring_data.monitored_transactions_seq,
                     pn->conf.monitored_transactions_sequence_size * 4);

  if ((ret = iota_consensus_cw_rating_init(&pn->cw_calc,
                                           DFS_FROM_ENTRY_POINT)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_consensus_ep_randomizer_init(
           &pn->ep_prob_map_randomizer, &pn->consensus->conf,
           EP_RANDOMIZE_MAP_AND_SAMPLE)) != RC_OK) {
    return ret;
  }

  neighbor_t *elt = NULL;
  neighbor_t *tmp = NULL;

  LL_FOREACH(neighbors, elt) {
    if ((ret = neighbors_add(&pn->neighbors, elt)) != RC_OK) {
      return ret;
    }
  }

  pn->monitoring_data.monitored_neighbor = NULL;
  pn->monitoring_data.monitored_transactions_seq = NULL;
  pn->monitoring_data.monitoring_start_timestamp = 0;
  pn->monitoring_data.txs_from_monitored_neighbor = NULL;
  pn->monitoring_data.txs_sequence_from_all_neighbors = NULL;

  // TODO - randomize next time
  pn->monitoring_data.monitoring_next_timestamp =
      current_timestamp_ms() + pn->conf.monitoring_interval_seconds;

  return RC_OK;
}
retcode_t iota_perceptive_node_start(iota_perceptive_node_t *const pn) {
  if (pn == NULL) {
    return RC_NULL_PARAM;
  }

  log_info(PERCEPTIVE_NODE_LOGGER_ID, "Starting perceptive node\n");

  // This function is noop atm

  return RC_OK;
}
retcode_t iota_perceptive_node_stop(iota_perceptive_node_t *const pn) {
  if (pn == NULL) {
    return RC_NULL_PARAM;
  } else if (pn->test_thread_running == false) {
    return RC_OK;
  }

  log_info(PERCEPTIVE_NODE_LOGGER_ID, "Shutting down perceptive node thread\n");
  pn->test_thread_running = false;
  if (thread_handle_join(pn->thread, NULL) != 0) {
    log_error(PERCEPTIVE_NODE_LOGGER_ID,
              "Shutting down perceptive_node thread failed\n");
    return RC_FAILED_THREAD_JOIN;
  }

  return RC_OK;
}

retcode_t iota_perceptive_node_destroy(iota_perceptive_node_t *const pn) {
  retcode_t ret;

  if (pn == NULL) {
    return RC_NULL_PARAM;
  } else if (pn->test_thread_running) {
    return RC_PERCEPTIVE_NODE_STILL_RUNNING;
  }

  if ((ret = iota_consensus_ep_randomizer_destroy(
           &pn->ep_prob_map_randomizer)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_consensus_cw_rating_destroy(&pn->cw_calc)) != RC_OK) {
    return ret;
  }
  destroy_monitoring_data(pn);
  logger_helper_destroy(PERCEPTIVE_NODE_LOGGER_ID);

  return RC_OK;
}

retcode_t iota_perceptive_node_on_next_transaction(
    iota_perceptive_node_t *const pn,
    iota_transaction_t const *const transaction, neighbor_t *const from) {
  retcode_t ret;
  uint64_t now = current_timestamp_ms();
  // Start a new sequence
  if (pn->monitoring_data.is_currently_monitoring == false &&
      now > pn->monitoring_data.monitoring_next_timestamp) {
    pn->monitoring_data.is_currently_monitoring = true;
    // TODO - We should randomize the next monitored neighbor
    // based on its TPS relative to overall TPS instead
    pn->monitoring_data.monitored_neighbor = from;
    pn->monitoring_data.monitoring_start_timestamp = now;
    // TODO - randomize next timestamp
    pn->monitoring_data.monitoring_next_timestamp =
        now + pn->conf.monitoring_interval_seconds;
    // TODO - depth
    if ((ret = iota_consensus_entry_point_selector_get_entry_point(
             &pn->consensus->entry_point_selector, pn->tangle, 3,
             pn->monitoring_data.entry_point)) != RC_OK) {
      return ret;
    }
  }

  if (neighbor_cmp(from, pn->monitoring_data.monitored_neighbor) == 0) {
    hash243_set_add(&pn->monitoring_data.txs_from_monitored_neighbor,
                    transaction_hash(transaction));
  }

  hash_array_reserve(pn->monitoring_data.txs_sequence_from_all_neighbors,
                     transaction_hash(transaction));

  if (hash243_set_size(pn->monitoring_data.txs_from_monitored_neighbor) ==
          pn->conf.test_sample_size &&
      pn->test_thread_running == false) {
    if (thread_handle_create(
            &pn->thread, (thread_routine_t)perceptive_node_do_test, pn) != 0) {
      return RC_FAILED_THREAD_SPAWN;
    }
  } else if (hash_array_len(
                 pn->monitoring_data.txs_sequence_from_all_neighbors) ==
             pn->conf.test_sample_size * 4) {
    // rollback, start again
  } else if (pn->test_thread_running) {
    log_warning(
        PERCEPTIVE_NODE_LOGGER_ID,
        "Can't start another test when one is already running, maybe increase "
        "\"monitoring_interval_seconds\"? \n");
  }

  return RC_OK;
}
