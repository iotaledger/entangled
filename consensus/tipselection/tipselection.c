/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/tipselection/tipselection.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/milestone/milestone.h"
#include "consensus/walker/walker.h"
#include "consensus/walker_validator/walker_validator.h"

#include "utils/logger_helper.h"

#define TIPSELECTION_LOGGER_ID "consensus_tipselection"

retcode_t iota_consensus_tipselection_init(
    tipselection_t *const ts, cw_rating_calculator_t *const cw_calc,
    entry_point_selector_t *const ep, ledger_validator_t *const lv,
    milestone_t *const milestone, tangle_t *const tangle,
    walker_t *const walker, walker_validator_t *const wv) {
  logger_helper_init(TIPSELECTION_LOGGER_ID, LOGGER_INFO, true);
  ts->cw_calc = cw_calc;
  ts->ep_selector = ep;
  ts->lv = lv;
  ts->milestone = milestone;
  ts->tangle = tangle;
  ts->walker = walker;
  rw_lock_handle_init(&ts->milestone->latest_snapshot.rw_lock);
  return RC_OK;
}

retcode_t iota_consensus_get_transactions_to_approve(
    tipselection_t *const ts, size_t const depth, trit_array_p const reference,
    tips_pair *tips) {
  retcode_t res = RC_OK;
  trit_array_p ep;
  cw_calc_result ratings_result;
  walker_validator_t wv;

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

  if ((res = iota_consensus_walker_validator_init(ts->tangle, ts->milestone,
                                                  ts->lv, &wv))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Initializing walker validator failed with error %\" PRIu64 \"\n",
              res);
    goto ret;
  }

  if ((res = iota_consensus_walker_walk(
           ts->walker, ep, ratings_result.cw_ratings, &wv, tips->trunk))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "First walking failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

  if (reference != NULL) {
    // TODO
  }

  if ((res = iota_consensus_walker_walk(
           ts->walker, ep, ratings_result.cw_ratings, &wv, tips->branch))) {
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
  ts->walker = NULL;
  rw_lock_handle_destroy(&ts->milestone->latest_snapshot.rw_lock);
  return RC_OK;
}
