#include "consensus/tipselection/tipselection.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/milestone/milestone.h"
#include "consensus/walker/walker.h"
#include "consensus/walker_validator/walker_validator.h"

#include "utils/logger_helper.h"

#define TIPSELECTION_LOGGER_ID "consensus_tipselection"

retcode_t iota_consensus_tipselection_init(
    tipselection_t *impl, const tangle_t *tangle, const ledger_validator_t *lv,
    const walker_validator_t *wv, const cw_rating_calculator_t *cw_calc,
    const milestone_t *milestone, const entry_point_selector_t *ep,
    const walker_t *walker) {
  logger_helper_init(TIPSELECTION_LOGGER_ID, LOGGER_INFO, true);
  impl->cw_calc = cw_calc;
  impl->milestone = milestone;
  impl->tangle = tangle;
  impl->lv = lv;
  impl->wv = wv;
  impl->walker = walker;
  rw_lock_handle_init(&impl->milestone->latest_snapshot.rw_lock);
  return RC_OK;
}

retcode_t iota_consensus_get_transactions_to_approve(
    tipselection_t *impl, size_t depth, const trit_array_p reference,
    tips_pair *tips) {
  retcode_t res = RC_OK;
  trit_array_p ep ;

  rw_lock_handle_rdlock(&impl->milestone->latest_snapshot.rw_lock);

  res = iota_consensus_get_entry_point(impl->ep_selector, depth, ep);

  cw_calc_result ratings_result;
  res = iota_consensus_cw_rating_calculate(impl->cw_calc, ep, &ratings_result);
  if (res != RC_OK) {
    // TODO
    rw_lock_handle_unlock(&impl->milestone->latest_snapshot.rw_lock);
  }

  walker_validator_t wv;

  iota_consensus_walker_validator_init(impl->tangle, impl->milestone, impl->lv,
                                       impl->wv);

  iota_consensus_walker_walk(impl->walker, ep, ratings_result.cw_ratings, &wv, tips->trunk);

  if (reference != NULL) {
    // TODO
  }

  iota_consensus_walker_walk(impl->walker, ep, ratings_result.cw_ratings, &wv, tips->branch);

  res = iota_consensus_ledeger_validator_validate(impl->lv, tips);
  if (res != RC_OK) {
    return res;
  }

  rw_lock_handle_unlock(&impl->milestone->latest_snapshot.rw_lock);

  return res;
}

retcode_t iota_consensus_tipselection_destroy() {
  logger_helper_destroy(TIPSELECTION_LOGGER_ID);
}
