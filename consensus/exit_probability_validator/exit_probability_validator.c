/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "utarray.h"
#include "utils/logger_helper.h"

#define WALKER_VALIDATOR_LOGGER_ID "consensus_walker_validator"

retcode_t iota_consensus_exit_prob_transaction_validator_init(
    iota_consensus_conf_t *const conf, tangle_t *const tangle,
    milestone_tracker_t *const mt, ledger_validator_t *const lv,
    exit_prob_transaction_validator_t *epv) {
  logger_helper_init(WALKER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  epv->conf = conf;
  epv->tangle = tangle;
  epv->mt = mt;
  epv->lv = lv;
  epv->max_depth_ok_memoization = NULL;
  return RC_OK;
}

retcode_t iota_consensus_exit_prob_transaction_validator_destroy(
    exit_prob_transaction_validator_t *epv) {
  logger_helper_destroy(WALKER_VALIDATOR_LOGGER_ID);

  hash243_set_free(&epv->max_depth_ok_memoization);
  hash243_set_free(&epv->analyzed_hashes);
  state_delta_destroy(&epv->delta);
  epv->delta = NULL;
  epv->tangle = NULL;
  epv->mt = NULL;
  epv->lv = NULL;

  return RC_OK;
}

retcode_t iota_consensus_exit_prob_transaction_validator_is_valid(
    exit_prob_transaction_validator_t *const epv,
    trit_array_t const *const tail_hash, bool *const is_valid) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_TX(tx, tx_models, tx_pack);

  ret = iota_tangle_transaction_load(epv->tangle, TRANSACTION_FIELD_HASH,
                                     tail_hash, &tx_pack);
  if (ret != RC_OK) {
    *is_valid = false;
    return ret;
  }

  if (tx_pack.num_loaded == 0) {
    *is_valid = false;
    log_error(WALKER_VALIDATOR_LOGGER_ID,
              "Validation failed, transaction is missing in db\n");
    return RC_OK;
  }

  if (tx.current_index != 0) {
    log_error(WALKER_VALIDATOR_LOGGER_ID,
              "Validation failed, transaction is not a tail\n");
    *is_valid = false;
    return RC_OK;
  }

  ret = iota_consensus_ledger_validator_update_delta(
      epv->lv, &epv->analyzed_hashes, &epv->delta, tail_hash->trits, is_valid);
  if (ret != RC_OK) {
    *is_valid = false;
    return ret;
  }
  if (!*is_valid) {
    log_error(WALKER_VALIDATOR_LOGGER_ID,
              "Validation failed, tail is inconsistent\n");
    return RC_OK;
  }

  bool below_max_depth = false;

  ret = iota_consensus_exit_prob_transaction_validator_below_max_depth(
      epv, tail_hash,
      epv->mt->latest_solid_subtangle_milestone_index - epv->conf->max_depth,
      &below_max_depth);
  if (ret != RC_OK) {
    return ret;
  }

  if (below_max_depth) {
    *is_valid = false;
    log_error(WALKER_VALIDATOR_LOGGER_ID,
              "Validation failed, tail is below max depth\n");
    return RC_OK;
  }

  *is_valid = true;

  return ret;
}

retcode_t iota_consensus_exit_prob_transaction_validator_below_max_depth(
    exit_prob_transaction_validator_t *epv, trit_array_t const *const tail_hash,
    uint32_t lowest_allowed_depth, bool *below_max_depth) {
  retcode_t res = RC_OK;

  hash243_stack_t non_analyzed_hashes = NULL;
  if ((res = hash243_stack_push(&non_analyzed_hashes, tail_hash->trits)) !=
      RC_OK) {
    return res;
  }

  // Load the transaction
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);

  hash243_set_t visited_hashes = NULL;
  TRIT_ARRAY_DECLARE(hash_trits_array, NUM_TRITS_HASH);

  while (non_analyzed_hashes != NULL) {
    if (hash243_set_size(&visited_hashes) == epv->conf->below_max_depth) {
      log_error(WALKER_VALIDATOR_LOGGER_ID,
                "Validation failed, exceeded num of transactions\n");
      *below_max_depth = false;
      break;
    }

    flex_trit_t *curr_hash_trits = hash243_stack_peek(non_analyzed_hashes);
    if (hash243_set_contains(&visited_hashes, curr_hash_trits)) {
      continue;
    }

    // Mark the transaction as visited
    if ((res = hash243_set_add(&visited_hashes, curr_hash_trits))) {
      break;
    }

    hash_trits_array.trits = curr_hash_trits;

    res = iota_tangle_transaction_load(epv->tangle, TRANSACTION_FIELD_HASH,
                                       &hash_trits_array, &pack);
    bool tail_is_not_genesis = (curr_tx_s.snapshot_index != 0 ||
                                memcmp(epv->conf->genesis_hash, curr_tx_s.hash,
                                       FLEX_TRIT_SIZE_243) == 0);
    if (tail_is_not_genesis &&
        (curr_tx_s.snapshot_index < lowest_allowed_depth)) {
      log_error(WALKER_VALIDATOR_LOGGER_ID,
                "Validation failed, transaction is below max depth\n");
      *below_max_depth = true;
      break;
    }
    if (curr_tx->snapshot_index == 0) {
      if ((res = hash243_stack_push(&non_analyzed_hashes, curr_tx->trunk)) !=
          RC_OK) {
        return res;
      }
      if ((res = hash243_stack_push(&non_analyzed_hashes, curr_tx->branch)) !=
          RC_OK) {
        return res;
      }
    }
    hash243_stack_pop(&non_analyzed_hashes);
  }

  hash243_stack_free(&non_analyzed_hashes);
  hash243_set_free(&visited_hashes);
  if ((res = hash243_set_add(&epv->max_depth_ok_memoization,
                             tail_hash->trits)) != RC_OK) {
    return res;
  }

  return res;
}
