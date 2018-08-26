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
    tipselection_t *impl, const cw_rating_calculator_t *cw_calc,
    const entry_point_selector_t *ep, const ledger_validator_t *lv,
    const milestone_t *milestone, const tangle_t *tangle,
    const walker_t *walker, const walker_validator_t *wv) {
  logger_helper_init(TIPSELECTION_LOGGER_ID, LOGGER_INFO, true);
  impl->cw_calc = cw_calc;
  impl->ep_selector = ep;
  impl->lv = lv;
  impl->milestone = milestone;
  impl->tangle = tangle;
  impl->walker = walker;
  rw_lock_handle_init(&impl->milestone->latest_snapshot.rw_lock);
  return RC_OK;
}

retcode_t iota_consensus_get_transactions_to_approve(
    tipselection_t *impl, size_t depth, const trit_array_p reference,
    tips_pair *tips) {
  retcode_t res = RC_OK;
  trit_array_p ep;
  cw_calc_result ratings_result;
  walker_validator_t wv;

  rw_lock_handle_rdlock(&impl->milestone->latest_snapshot.rw_lock);

  if ((res = iota_consensus_get_entry_point(impl->ep_selector, depth, ep))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Getting entry point failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

  if ((res = iota_consensus_cw_rating_calculate(impl->cw_calc, ep,
                                                &ratings_result))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Calculating CW ratings failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

  if ((res = iota_consensus_walker_validator_init(impl->tangle, impl->milestone,
                                                  impl->lv, &wv))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Initializing walker validator failed with error %\" PRIu64 \"\n",
              res);
    goto ret;
  }

  if ((res = iota_consensus_walker_walk(
           impl->walker, ep, ratings_result.cw_ratings, &wv, tips->trunk))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "First walking failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

  if (reference != NULL) {
    // TODO
  }

  if ((res = iota_consensus_walker_walk(
           impl->walker, ep, ratings_result.cw_ratings, &wv, tips->branch))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Second walking failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

  if ((res = iota_consensus_ledeger_validator_validate(impl->lv, tips))) {
    log_error(TIPSELECTION_LOGGER_ID,
              "Validating tips failed with error %\" PRIu64 \"\n", res);
    goto ret;
  }

ret:
  rw_lock_handle_unlock(&impl->milestone->latest_snapshot.rw_lock);
  return res;
}

retcode_t iota_consensus_tipselection_destroy(tipselection_t *impl) {
  logger_helper_destroy(TIPSELECTION_LOGGER_ID);
  impl->cw_calc = NULL;
  impl->ep_selector = NULL;
  impl->lv = NULL;
  impl->milestone = NULL;
  impl->tangle = NULL;
  impl->walker = NULL;
  rw_lock_handle_destroy(&impl->milestone->latest_snapshot.rw_lock);
  return RC_OK;
}
