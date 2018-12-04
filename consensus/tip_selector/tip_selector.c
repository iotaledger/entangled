/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/exit_probability_randomizer/exit_probability_randomizer.h"
#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "consensus/snapshot/snapshot.h"
#include "consensus/tip_selector/tip_selector.h"
#include "utils/logger_helper.h"

#define TIP_SELECTOR_LOGGER_ID "consensus_tip_selector"

retcode_t iota_consensus_tip_selector_init(
    tip_selector_t *const tip_selector, iota_consensus_conf_t *const conf,
    cw_rating_calculator_t *const cw_rating_calculator,
    entry_point_selector_t *const entry_point_selector,
    ep_randomizer_t *const ep_randomizer,
    exit_prob_transaction_validator_t *const walker_validator,
    ledger_validator_t *const ledger_validator,
    milestone_tracker_t *const milestone_tracker, tangle_t *const tangle) {
  tip_selector->conf = conf;
  tip_selector->cw_rating_calculator = cw_rating_calculator;
  tip_selector->entry_point_selector = entry_point_selector;
  tip_selector->ep_randomizer = ep_randomizer;
  tip_selector->walker_validator = walker_validator;
  tip_selector->ledger_validator = ledger_validator;
  tip_selector->milestone_tracker = milestone_tracker;
  tip_selector->tangle = tangle;
  return RC_OK;
}

retcode_t iota_consensus_tip_selector_get_transactions_to_approve(
    tip_selector_t *const tip_selector, size_t const depth,
    trit_array_t const *const reference, tips_pair *const tips) {
  retcode_t ret = RC_OK;
  flex_trit_t ep_trits[FLEX_TRIT_SIZE_243];
  trit_array_t ep = {.trits = ep_trits,
                     .num_trits = HASH_LENGTH_TRIT,
                     .num_bytes = FLEX_TRIT_SIZE_243,
                     .dynamic = 0};
  cw_calc_result rating_results = {.cw_ratings = NULL, .tx_to_approvers = NULL};
  bool consistent = false;
  hash243_stack_t tips_stack = NULL;

  rw_lock_handle_rdlock(
      &tip_selector->milestone_tracker->latest_snapshot->rw_lock);

  if ((ret = iota_consensus_entry_point_selector_get_entry_point(
           tip_selector->entry_point_selector, depth, &ep)) != RC_OK) {
    log_error(TIP_SELECTOR_LOGGER_ID,
              "Getting entry point failed with error %" PRIu64 "\n", ret);
    goto done;
  }

  if ((ret = iota_consensus_cw_rating_calculate(
           tip_selector->cw_rating_calculator, &ep, &rating_results)) !=
      RC_OK) {
    log_error(TIP_SELECTOR_LOGGER_ID,
              "Calculating CW ratings failed with error %" PRIu64 "\n", ret);
    goto done;
  }

  if ((ret = iota_consensus_exit_probability_randomize(
           tip_selector->ep_randomizer, tip_selector->walker_validator,
           &rating_results, &ep, tips->trunk)) != RC_OK) {
    log_error(TIP_SELECTOR_LOGGER_ID,
              "Getting trunk tip failed with error %" PRIu64 "\n", ret);
    goto done;
  }
  if ((ret = hash243_stack_push(&tips_stack, tips->trunk->trits)) != RC_OK) {
    goto done;
  }

  if (reference != NULL && reference->trits != NULL) {
    if (!hash_int_map_contains(&rating_results.cw_ratings, reference->trits)) {
      log_warning(TIP_SELECTOR_LOGGER_ID, "Reference is too old\n");
      ret = RC_TIP_SELECTOR_REFERENCE_TOO_OLD;
      goto done;
    }
    ep.trits = reference->trits;
  }

  if ((ret = iota_consensus_exit_probability_randomize(
           tip_selector->ep_randomizer, tip_selector->walker_validator,
           &rating_results, &ep, tips->branch)) != RC_OK) {
    log_error(TIP_SELECTOR_LOGGER_ID,
              "Getting branch tip failed with error %" PRIu64 "\n", ret);
    goto done;
  }
  if ((ret = hash243_stack_push(&tips_stack, tips->branch->trits)) != RC_OK) {
    goto done;
  }

  if ((ret = iota_consensus_ledger_validator_check_consistency(
           tip_selector->ledger_validator, tips_stack, &consistent)) != RC_OK) {
    log_error(TIP_SELECTOR_LOGGER_ID,
              "Checking consistency of tips failed with error %" PRIu64 "\n",
              ret);
    goto done;
  }

  if (!consistent) {
    log_warning(TIP_SELECTOR_LOGGER_ID, "Tips are not consistent\n");
    ret = RC_TIP_SELECTOR_TIPS_NOT_CONSISTENT;
  }

done:
  rw_lock_handle_unlock(
      &tip_selector->milestone_tracker->latest_snapshot->rw_lock);
  cw_calc_result_destroy(&rating_results);
  hash243_stack_free(&tips_stack);
  return ret;
}

retcode_t iota_consensus_tip_selector_destroy(
    tip_selector_t *const tip_selector) {
  tip_selector->cw_rating_calculator = NULL;
  tip_selector->entry_point_selector = NULL;
  tip_selector->ep_randomizer = NULL;
  tip_selector->walker_validator = NULL;
  tip_selector->ledger_validator = NULL;
  tip_selector->milestone_tracker = NULL;
  tip_selector->tangle = NULL;
  logger_helper_destroy(TIP_SELECTOR_LOGGER_ID);
  return RC_OK;
}
