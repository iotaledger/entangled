/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/perceptive_node/perceptive_node.h"
#include <float.h>
#include <math.h>
#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"
#include "consensus/utils/tangle_simulator.h"
#include "utils/handles/rand.h"
#include "utils/macros.h"

#define PERCEPTIVE_NODE_LOGGER_ID "perceptive_node"

/*
 * Private functions
 */

static void clear_monitoring_data(iota_perceptive_node_t *const pn) {
  hash_to_hash_pair_t_map_free(
      &pn->monitoring_data.txs_from_monitored_neighbor);
  utarray_clear(pn->monitoring_data.txs_sequence_from_all_neighbors);
  hash_to_indexed_hash_set_map_free(pn->monitoring_data.base_approvers_map);
  pn->monitoring_data.txs_from_monitored_neighbor_size = 0;
}

static void reset_neighbors_stats(iota_perceptive_node_t *const pn) {
  neighbor_t_to_uint32_t_map_set_all(
      &pn->neighbors_to_recent_transactions_count, 0);
}

static void destroy_monitoring_data(iota_perceptive_node_t *const pn) {
  hash_to_hash_pair_t_map_free(
      &pn->monitoring_data.txs_from_monitored_neighbor);
  hash_array_free(pn->monitoring_data.txs_sequence_from_all_neighbors);
  hash_to_indexed_hash_set_map_free(pn->monitoring_data.base_approvers_map);
  pn->monitoring_data.txs_from_monitored_neighbor_size = 0;
}

static void randomize_next_monitored_neighbor(
    iota_perceptive_node_t *const pn) {
  neighbor_t_to_uint32_t_map_entry_t *curr_entry = NULL;
  neighbor_t_to_uint32_t_map_entry_t *tmp_entry = NULL;

  uint32_t total_transactions_count = 0;
  HASH_ITER(hh, pn->neighbors_to_recent_transactions_count, curr_entry,
            tmp_entry) {
    total_transactions_count += curr_entry->value;
  }

  double randomized_target =
      rand_handle_probability() * total_transactions_count;
  curr_entry = NULL;
  HASH_ITER(hh, pn->neighbors_to_recent_transactions_count, curr_entry,
            tmp_entry) {
    randomized_target -= curr_entry->value;
    if (randomized_target <= 0) {
      memcpy(&pn->monitoring_data.monitored_neighbor, &curr_entry->key,
             sizeof(neighbor_t));
      break;
    }
  }
  reset_neighbors_stats(pn);
}

static void randomize_next_monitoring_timestamp(
    iota_perceptive_node_t *const pn, int64_t now) {
  pn->monitoring_data.monitoring_next_timestamp =
      now + rand_handle_probability() * pn->conf.monitoring_interval_seconds +
      pn->conf.monitoring_interval_seconds;
}

static double calculate_current_tip_attachment_prob(
    hash_to_double_map_t const *const trans_probs,
    cw_calc_result const *const cw_result,
    hash_to_hash_pair_t_map_entry_t const *const map_entry) {
  hash_to_double_map_entry_t *exit_prob_entry = NULL;
  double prob = 1.0;

  if (memcmp(map_entry->value.one, map_entry->value.two, FLEX_TRIT_SIZE_243) ==
      0) {
    hash_to_double_map_find(trans_probs, map_entry->value.one,
                            &exit_prob_entry);
    prob *= exit_prob_entry->value * exit_prob_entry->value;
  } else {
    hash_to_double_map_find(trans_probs, map_entry->value.one,
                            &exit_prob_entry);
    prob *= exit_prob_entry->value;
    hash_to_double_map_find(trans_probs, map_entry->value.two,
                            &exit_prob_entry);
    prob *= exit_prob_entry->value;
    prob *= 2;
  }
  // TODO - penalize prob based on whether or not it's a tip, (the "dipper" a tx
  // TODO - branch/trunk's is, the larger the penalty is (That's what cw_result
  // TODO - is for)
  return prob;
}

static retcode_t calculate_lf_prob_value(iota_perceptive_node_t *const pn,
                                         double *const lf_prob) {
  retcode_t ret = RC_OK;
  *lf_prob = 1.0;
  flex_trit_t *curr_tx_hash;
  perceptive_node_monitoring_data_t *md = &pn->monitoring_data;
  uint32_t curr_subtangle_size = md->subtangle_size;
  cw_calc_result cw_ratings;

  ep_prob_map_randomizer_t ep_prob_map_randomizer;
  hash_to_hash_pair_t_map_entry_t *curr_tip_entry = NULL;
  hash_to_indexed_hash_set_map_t approvers_map = NULL;

  if ((ret = iota_consensus_ep_randomizer_init(
           &ep_prob_map_randomizer, &pn->consensus->conf,
           EP_RANDOMIZE_MAP_AND_SAMPLE)) != RC_OK) {
    return ret;
  }

  if ((ret = hash_to_indexed_hash_set_map_clone(md->base_approvers_map,
                                                &approvers_map)) != RC_OK) {
    return ret;
  }
  // Initial cumulative weights calculation
  if ((ret = iota_consensus_cw_rating_calculate(&pn->cw_calc, pn->tangle,
                                                pn->monitoring_data.entry_point,
                                                &cw_ratings)) != RC_OK) {
    goto cleanup;
  }

  // This loop iterate through the received transactions sequence in the order
  // of which they were received and for each transaction, if it belongs
  // to the node we currently are monitoring, we calculate the probability
  // of that tip being attached on both branch and trunk and update our lf_prob
  for (size_t i = 0; i < hash_array_len(md->txs_sequence_from_all_neighbors);
       ++i) {
    curr_tx_hash = hash_array_at(md->txs_sequence_from_all_neighbors, i);
    if (hash_to_hash_pair_t_map_find(&md->txs_from_monitored_neighbor,
                                     curr_tx_hash, &curr_tip_entry)) {
      if ((ret = iota_consensus_exit_prob_map_calculate_probs(
               &ep_prob_map_randomizer, pn->tangle,
               &pn->consensus->exit_prob_transaction_validator, &cw_ratings,
               pn->monitoring_data.entry_point,
               &ep_prob_map_randomizer.transition_probs,
               &ep_prob_map_randomizer.exit_probs)) != RC_OK) {
        goto cleanup;
      }

      *lf_prob *= pow(calculate_current_tip_attachment_prob(
                          &ep_prob_map_randomizer.transition_probs, &cw_ratings,
                          curr_tip_entry),
                      1 / (double)pn->conf.test_sample_size);
    }

    if ((ret = tangle_simulator_add_transaction_recalc_ratings(
             curr_subtangle_size++, curr_tx_hash, curr_tip_entry->value.one,
             curr_tip_entry->value.two, &approvers_map,
             &cw_ratings.cw_ratings)) != RC_OK) {
      goto cleanup;
    }
  }
cleanup:
  iota_consensus_exit_prob_map_destroy(&ep_prob_map_randomizer);
  hash_to_indexed_hash_set_map_free(&approvers_map);
  return ret;
}

static retcode_t calculate_samples_from_lf_distribution(
    iota_perceptive_node_t *const pn) {
  retcode_t ret = RC_OK;
  flex_trit_t *curr_tx_hash;
  perceptive_node_monitoring_data_t *md = &pn->monitoring_data;
  uint32_t curr_subtangle_size;
  uint32_t orig_subtangle_size = md->subtangle_size;
  flex_trit_t curr_selected_tip[FLEX_TRIT_SIZE_243];
  cw_calc_result cw_ratings;
  double curr_lf_prob;

  ep_prob_map_randomizer_t ep_prob_map_randomizer;
  hash_to_hash_pair_t_map_entry_t *curr_tip_entry = NULL;

  hash_to_indexed_hash_set_map_t approvers_map = NULL;

  if ((ret = iota_consensus_ep_randomizer_init(
           &ep_prob_map_randomizer, &pn->consensus->conf,
           EP_RANDOMIZE_MAP_AND_SAMPLE)) != RC_OK) {
    return ret;
  }

  if ((ret = hash_to_indexed_hash_set_map_clone(md->base_approvers_map,
                                                &approvers_map)) != RC_OK) {
    return ret;
  }
  // Initial cumulative weights calculation
  if ((ret = iota_consensus_cw_rating_calculate(&pn->cw_calc, pn->tangle,
                                                pn->monitoring_data.entry_point,
                                                &cw_ratings)) != RC_OK) {
    goto cleanup;
  }

  for (size_t sample_idx = 0; sample_idx < pn->conf.test_sample_size;
       ++sample_idx) {
    curr_lf_prob = 1.0;
    curr_subtangle_size = orig_subtangle_size;
    // This loop iterate through the received transactions sequence in the
    // order of which they were received and for each transaction, if it
    // belongs to the node we currently are monitoring, we calculate the
    // probability of that tip being attached on both branch and trunk and
    // update our lf_score
    for (size_t i = 0; i < hash_array_len(md->txs_sequence_from_all_neighbors);
         ++i) {
      curr_tx_hash = hash_array_at(md->txs_sequence_from_all_neighbors, i);
      if (hash_to_hash_pair_t_map_find(&md->txs_from_monitored_neighbor,
                                       curr_tx_hash, &curr_tip_entry)) {
        if ((ret = iota_consensus_exit_prob_map_calculate_probs(
                 &ep_prob_map_randomizer, pn->tangle,
                 &pn->consensus->exit_prob_transaction_validator, &cw_ratings,
                 pn->monitoring_data.entry_point,
                 &ep_prob_map_randomizer.transition_probs,
                 &ep_prob_map_randomizer.exit_probs)) != RC_OK) {
          goto cleanup;
        }
        // Get first tip (trunk)
        if ((ret = iota_consensus_exit_prob_map_randomize(
                 &ep_prob_map_randomizer, pn->tangle,
                 &pn->consensus->exit_prob_transaction_validator, &cw_ratings,
                 pn->monitoring_data.entry_point, curr_tip_entry->value.one)) !=
            RC_OK) {
          return ret;
        }
        // Get second tip (branch)
        if ((ret = iota_consensus_exit_prob_map_randomize(
                 &ep_prob_map_randomizer, pn->tangle,
                 &pn->consensus->exit_prob_transaction_validator, &cw_ratings,
                 pn->monitoring_data.entry_point, curr_tip_entry->value.two)) !=
            RC_OK) {
          return ret;
        }

        curr_lf_prob *= pow(calculate_current_tip_attachment_prob(
                                &ep_prob_map_randomizer.transition_probs,
                                &cw_ratings, curr_tip_entry),
                            1 / (double)pn->conf.test_sample_size);
      }
      // Update tangle with newly attached transaction
      if ((ret = tangle_simulator_add_transaction_recalc_ratings(
               curr_subtangle_size++, curr_selected_tip,
               curr_tip_entry->value.one, curr_tip_entry->value.two,
               &approvers_map, &cw_ratings.cw_ratings)) != RC_OK) {
        goto cleanup;
      }
    }

    double_array_push(pn->monitoring_data.test_lf_distribution_samples,
                      curr_lf_prob);
  }

cleanup:
  iota_consensus_exit_prob_map_destroy(&ep_prob_map_randomizer);
  hash_to_indexed_hash_set_map_free(&approvers_map);
  return ret;
}

static double calculate_r_score_from_lf_prob(iota_perceptive_node_t *const pn,
                                             double lf_prob) {
  size_t in_range_count = 0;
  double *curr_sample_prob;
  for (size_t i = 0; i < pn->conf.test_sample_size; i++) {
    *curr_sample_prob =
        double_array_at(&pn->monitoring_data.test_lf_distribution_samples, i);
    if (*curr_sample_prob < lf_prob) {
      ++in_range_count;
    }
  }

  double in_range_ratio =
      (double)(in_range_count) / (double)pn->conf.test_sample_size;

  return 2 * MIN(in_range_ratio, 1 - in_range_ratio);
}

static void *perceptive_node_do_test(iota_perceptive_node_t *const pn) {
  double test_r_score;
  pn->test_thread_running = true;
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

  double lf_score;
  if (calculate_lf_prob_value(pn, &lf_score) != RC_OK) {
    log_error(PERCEPTIVE_NODE_LOGGER_ID,
              "In %s, failed in LF score calculation\n", __FUNCTION__);
    goto cleanup;
  }

  if (calculate_samples_from_lf_distribution(pn) != RC_OK) {
    goto cleanup;
  }

  test_r_score = calculate_r_score_from_lf_prob(pn, lf_score);
  log_info(PERCEPTIVE_NODE_LOGGER_ID,
           "In %s, Finished test for node %s with r value: %.f\n", __FUNCTION__,
           pn->monitoring_data.monitored_neighbor.endpoint.host, DECIMAL_DIG,
           lf_score);

cleanup:
  double_array_free(pn->monitoring_data.test_lf_distribution_samples);
  pn->monitoring_data.test_lf_distribution_samples = NULL;
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
  pn->neighbors_to_recent_transactions_count = NULL;
  logger_helper_init(PERCEPTIVE_NODE_LOGGER_ID, LOGGER_DEBUG, true);
  pn->test_thread_running = false;
  pn->consensus = consensus;
  pn->monitoring_data.base_approvers_map = NULL;
  pn->monitoring_data.test_lf_distribution_samples = double_array_new();

  connection_config_t db_conf = {.db_path = pn->conf.db_path};
  if ((ret = iota_tangle_init(&pn->tangle, &db_conf)) != RC_OK) {
    log_critical(PERCEPTIVE_NODE_LOGGER_ID,
                 "Initializing tangle connection failed\n");
    return ret;
  }

  hash243_array_new(&pn->monitoring_data.monitored_transactions_seq);
  hash_array_reserve(&pn->monitoring_data.monitored_transactions_seq,
                     pn->conf.monitored_transactions_sequence_size /
                             pn->conf.min_hash_power_ratio +
                         1);
  double_array_reserve(pn->monitoring_data.test_lf_distribution_samples,
                       pn->conf.test_sample_size);

  if ((ret = iota_consensus_cw_rating_init(&pn->cw_calc,
                                           DFS_FROM_ENTRY_POINT)) != RC_OK) {
    return ret;
  }

  neighbor_t *elt = NULL;
  LL_FOREACH(neighbors, elt) {
    if ((ret = neighbors_add(&pn->neighbors, elt)) != RC_OK) {
      return ret;
    }
  }
  reset_neighbors_stats(pn);

  pn->monitoring_data.monitored_transactions_seq = NULL;
  pn->monitoring_data.monitoring_start_timestamp = 0;
  pn->monitoring_data.txs_from_monitored_neighbor = NULL;
  pn->monitoring_data.txs_sequence_from_all_neighbors = NULL;

  randomize_next_monitoring_timestamp(pn, current_timestamp_ms());

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

  if ((ret = iota_consensus_cw_rating_destroy(&pn->cw_calc)) != RC_OK) {
    return ret;
  }
  neighbor_t_to_uint32_t_map_free(&pn->neighbors_to_recent_transactions_count);
  destroy_monitoring_data(pn);
  logger_helper_destroy(PERCEPTIVE_NODE_LOGGER_ID);

  return RC_OK;
}

retcode_t iota_perceptive_node_on_next_transaction(
    iota_perceptive_node_t *const pn,
    iota_transaction_t const *const transaction, neighbor_t *const from) {
  retcode_t ret;
  uint64_t now = current_timestamp_ms();
  hash_pair_t attachment_point;
  uint32_t node_transaction_count;

  // If this is not a monitoring session period, keep on collecting
  // Data on neighbors
  if (pn->monitoring_data.is_currently_monitoring == false &&
      now < pn->monitoring_data.monitoring_next_timestamp) {
    node_transaction_count = neighbor_t_to_uint32_t_map_at(
        &pn->neighbors_to_recent_transactions_count, from);
    neighbor_t_to_uint32_t_map_set(&pn->neighbors_to_recent_transactions_count,
                                   from, node_transaction_count++);
    return RC_OK;
  }
  // Start a new sequence
  if (pn->monitoring_data.is_currently_monitoring == false &&
      now > pn->monitoring_data.monitoring_next_timestamp) {
    pn->monitoring_data.txs_from_monitored_neighbor_size = 0;
    pn->monitoring_data.is_currently_monitoring = true;
    randomize_next_monitored_neighbor(pn);
    pn->monitoring_data.monitoring_start_timestamp = now;
    randomize_next_monitoring_timestamp(pn, now);
    if ((ret = iota_consensus_entry_point_selector_get_entry_point(
             &pn->consensus->entry_point_selector, pn->tangle,
             pn->conf.random_walk_depth, pn->monitoring_data.entry_point)) !=
        RC_OK) {
      return ret;
    }
  }

  hash_array_push(pn->monitoring_data.txs_sequence_from_all_neighbors,
                  transaction_hash(transaction));
  // If current transaction is from the monitored node, then we want to store
  // it in the map so we can get it's LF value
  if (neighbor_cmp(from, &pn->monitoring_data.monitored_neighbor) == 0) {
    memcpy(attachment_point.one, transaction_trunk(transaction),
           FLEX_TRIT_SIZE_243);
    memcpy(attachment_point.two, transaction_branch(transaction),
           FLEX_TRIT_SIZE_243);
    hash_to_hash_pair_t_map_add(
        &pn->monitoring_data.txs_from_monitored_neighbor,
        transaction_hash(transaction), attachment_point);
    pn->monitoring_data.txs_from_monitored_neighbor_size++;
  }

  if (pn->monitoring_data.txs_from_monitored_neighbor_size ==
      pn->conf.test_sample_size) {
    if (pn->test_thread_running == false) {
      if (thread_handle_create(&pn->thread,
                               (thread_routine_t)perceptive_node_do_test,
                               pn) != 0) {
        return RC_FAILED_THREAD_SPAWN;
      }
    } else if (hash_array_len(
                   pn->monitoring_data.txs_sequence_from_all_neighbors) *
                   pn->conf.min_hash_power_ratio >=
               pn->conf.test_sample_size) {
      clear_monitoring_data(pn);
    } else if (pn->test_thread_running) {
      log_warning(PERCEPTIVE_NODE_LOGGER_ID,
                  "Can't start another test when one is already running, maybe "
                  "increase "
                  "\"monitoring_interval_seconds\"? \n");
    }
  }

  return RC_OK;
}
