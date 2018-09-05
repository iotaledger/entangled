/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/tipselection/tipselection.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/exit_probability_randomizer/exit_probability_randomizer.h"
#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "consensus/milestone/milestone.h"

#include "utils/logger_helper.h"

#define TIPSELECTION_LOGGER_ID "consensus_tipselection"

retcode_t iota_consensus_tipselection_init(

    tipselection_t *impl, const tangle_t *tangle, const ledger_validator_t *lv,
    const exit_prob_transaction_validator_t *wv,
    const cw_rating_calculator_t *cw_calc, const milestone_t *milestone,
    const entry_point_selector_t *ep, const ep_randomizer_t *ep_randomizer,
    double alpha) {
  logger_helper_init(TIPSELECTION_LOGGER_ID, LOGGER_INFO, true);
  impl->cw_calc = cw_calc;
  impl->milestone = milestone;
  impl->tangle = tangle;
  impl->lv = lv;
  impl->wv = wv;
  impl->ep_randomizer = ep_randomizer;
  impl->ep_selector = ep;
  rw_lock_handle_init(&impl->milestone->latest_snapshot.rw_lock);
  return RC_OK;
}

retcode_t iota_consensus_get_transactions_to_approve(
    tipselection_t *const ts, size_t const depth, trit_array_p const reference,
    tips_pair *tips) {
  retcode_t res = RC_OK;
  trit_array_p ep = NULL;
  cw_calc_result ratings_result;

  rw_lock_handle_rdlock(&ts->milestone->latest_snapshot.rw_lock);

  if ((res = iota_consensus_get_entry_point(ts->ep_selector, depth, ep))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Getting entry point failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

  if ((res = iota_consensus_cw_rating_calculate(ts->cw_calc, ep,
                                                &ratings_result))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Calculating CW ratings failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

  exit_prob_transaction_validator_t wv;

  iota_consensus_exit_prob_transaction_validator_init(ts->tangle, ts->milestone,
                                                      ts->lv, ts->wv);

  iota_consensus_exit_probability_randomize(
      &ratings_result, ep, ratings_result.cw_ratings, &wv, tips->trunk);

  if (reference != NULL) {
    // TODO
  }

  iota_consensus_exit_probability_randomize(
      ts->ep_randomizer, ep, ratings_result.cw_ratings, &wv, tips->branch);

  res = iota_consensus_ledeger_validator_validate(ts->lv, tips);
  if (res != RC_OK) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Second walking failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

  if ((res = iota_consensus_ledeger_validator_validate(ts->lv, tips))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Validating tips failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

ret:
  rw_lock_handle_unlock(&ts->milestone->latest_snapshot.rw_lock);
  return res;
}

retcode_t iota_consensus_tipselection_destroy(tipselection_t *const ts) {
  logger_helper_destroy(TIPSELECTION_LOGGER_ID);
  ts->cw_calc = NULL;
  ts->ep_selector = NULL;
  ts->lv = NULL;
  ts->milestone = NULL;
  ts->tangle = NULL;
  ts->ep_randomizer = NULL;
  rw_lock_handle_destroy(&ts->milestone->latest_snapshot.rw_lock);
  return RC_OK;
}
