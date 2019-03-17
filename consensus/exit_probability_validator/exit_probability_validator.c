/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "utarray.h"
#include "utils/logger_helper.h"

#define WALKER_VALIDATOR_LOGGER_ID "walker_validator"

static logger_id_t logger_id;

/*
 * Private functions
 */

static retcode_t iota_consensus_exit_prob_transaction_validator_below_max_depth(exit_prob_transaction_validator_t *epv,
                                                                                tangle_t *const tangle,
                                                                                flex_trit_t const *const tail_hash,
                                                                                uint32_t lowest_allowed_depth,
                                                                                bool *below_max_depth) {
  retcode_t res = RC_OK;

  bool is_genesis_hash;
  flex_trit_t *curr_hash_trits;
  hash243_stack_t non_analyzed_hashes = NULL;
  if ((res = hash243_stack_push(&non_analyzed_hashes, tail_hash)) != RC_OK) {
    return res;
  }

  // Load the transaction
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, pack);

  hash243_set_t visited_hashes = NULL;
  uint32_t curr_snapshot_index;

  while (non_analyzed_hashes != NULL) {
    if (hash243_set_size(&visited_hashes) == epv->conf->below_max_depth) {
      log_error(logger_id, "Validation failed, exceeded num of transactions\n");
      *below_max_depth = true;
      break;
    }

    curr_hash_trits = hash243_stack_peek(non_analyzed_hashes);
    is_genesis_hash = (memcmp(epv->conf->genesis_hash, curr_hash_trits, FLEX_TRIT_SIZE_243) == 0);
    if (hash243_set_contains(&visited_hashes, curr_hash_trits)) {
      hash243_stack_pop(&non_analyzed_hashes);
      continue;
    }

    // Mark the transaction as visited
    if ((res = hash243_set_add(&visited_hashes, curr_hash_trits))) {
      break;
    }
    if (!is_genesis_hash) {
      hash_pack_reset(&pack);
      if ((res = iota_tangle_transaction_load_partial(tangle, curr_hash_trits, &pack,
                                                      PARTIAL_TX_MODEL_ESSENCE_ATTACHMENT_METADATA)) != RC_OK) {
        return res;
      }
      curr_snapshot_index = transaction_snapshot_index(&curr_tx_s);
    } else {
      curr_snapshot_index = 0;
    }

    if (curr_snapshot_index < lowest_allowed_depth) {
      log_error(logger_id, "Validation failed, transaction is below max depth\n");
      *below_max_depth = true;
      break;
    }
    hash243_stack_pop(&non_analyzed_hashes);
    if (!is_genesis_hash && transaction_snapshot_index(curr_tx) == 0) {
      if ((res = hash243_stack_push(&non_analyzed_hashes, transaction_trunk(curr_tx))) != RC_OK) {
        return res;
      }
      if ((res = hash243_stack_push(&non_analyzed_hashes, transaction_branch(curr_tx))) != RC_OK) {
        return res;
      }
    }
  }

  hash243_stack_free(&non_analyzed_hashes);
  hash243_set_free(&visited_hashes);
  if ((res = hash243_set_add(&epv->max_depth_ok_memoization, tail_hash)) != RC_OK) {
    return res;
  }

  return res;
}

/*
 * Public functions
 */

retcode_t iota_consensus_exit_prob_transaction_validator_init(iota_consensus_conf_t *const conf,
                                                              milestone_tracker_t *const mt,
                                                              ledger_validator_t *const lv,
                                                              exit_prob_transaction_validator_t *epv) {
  logger_id = logger_helper_enable(WALKER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  epv->conf = conf;
  epv->mt = mt;
  epv->lv = lv;
  epv->delta = NULL;
  epv->analyzed_hashes = NULL;
  epv->max_depth_ok_memoization = NULL;
  return RC_OK;
}

retcode_t iota_consensus_exit_prob_transaction_validator_destroy(exit_prob_transaction_validator_t *epv) {
  logger_helper_release(logger_id);

  hash243_set_free(&epv->max_depth_ok_memoization);
  hash243_set_free(&epv->analyzed_hashes);
  state_delta_destroy(&epv->delta);
  epv->delta = NULL;
  epv->mt = NULL;
  epv->lv = NULL;

  return RC_OK;
}

retcode_t iota_consensus_exit_prob_transaction_validator_is_valid(exit_prob_transaction_validator_t *const epv,
                                                                  tangle_t *const tangle,
                                                                  flex_trit_t const *const tail_hash,
                                                                  bool *const is_valid) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_TX(tx, tx_models, tx_pack);

  if ((ret = iota_tangle_transaction_load_partial(tangle, tail_hash, &tx_pack,
                                                  PARTIAL_TX_MODEL_ESSENCE_ATTACHMENT_METADATA)) != RC_OK) {
    *is_valid = false;
    return ret;
  }

  if (tx_pack.num_loaded == 0) {
    *is_valid = false;
    log_error(logger_id, "Validation failed, transaction is missing in db\n");
    return RC_OK;
  }

  if (transaction_current_index(&tx) != 0) {
    log_error(logger_id, "Validation failed, transaction is not a tail\n");
    *is_valid = false;
    return RC_OK;
  }

  if ((ret = iota_consensus_ledger_validator_update_delta(epv->lv, tangle, &epv->analyzed_hashes, &epv->delta,
                                                          tail_hash, is_valid)) != RC_OK) {
    *is_valid = false;
    return ret;
  }

  if (!*is_valid) {
    log_error(logger_id, "Validation failed, tail is inconsistent\n");
    return RC_OK;
  }

  bool below_max_depth = false;

  uint32_t lowest_allowed_depth = epv->mt->latest_solid_subtangle_milestone_index < epv->conf->max_depth
                                      ? epv->mt->latest_solid_subtangle_milestone_index
                                      : epv->mt->latest_solid_subtangle_milestone_index - epv->conf->max_depth;

  if ((ret = iota_consensus_exit_prob_transaction_validator_below_max_depth(
           epv, tangle, tail_hash, lowest_allowed_depth, &below_max_depth)) != RC_OK) {
    return ret;
  }

  if (below_max_depth) {
    *is_valid = false;
    log_error(logger_id, "Validation failed, tail is below max depth\n");
    return RC_OK;
  }

  *is_valid = true;

  return ret;
}
