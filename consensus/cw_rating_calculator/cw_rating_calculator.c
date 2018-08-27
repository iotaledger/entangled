/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "common/errors.h"
#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"

retcode_t iota_consensus_cw_rating_init(
    cw_rating_calculator_t *const cw_calc, tangle_t *const tangle,
    enum cw_calculation_implementation impl) {
  logger_helper_init(CW_RATING_CALCULATOR_LOGGER_ID, LOGGER_INFO, true);
  cw_calc->tangle = tangle;
  if (impl == DFS) {
    init_cw_calculator_dfs(&cw_calc->base);
  } else if (impl == NO_IMPLEMENTATION) {
    return RC_CONSENSUS_NOT_IMPLEMENTED;
  }
  return RC_OK;
}

retcode_t iota_consensus_cw_rating_destroy(cw_rating_calculator_t *cw_calc) {
  logger_helper_destroy(CW_RATING_CALCULATOR_LOGGER_ID);
  cw_calc->tangle = NULL;
  return R_OK;
}

retcode_t iota_consensus_cw_rating_calculate(
    const cw_rating_calculator_t *calculator, trit_array_p entry_point,
    cw_calc_result *out) {
  if (calculator->base.vtable.cw_rating_calculate == NULL) {
    log_error(CW_RATING_CALCULATOR_LOGGER_ID, "Vtable is not initialized\n");
    return RC_CONSENSUS_NULL_PTR;
  }
  return calculator->base.vtable.cw_rating_calculate(calculator, entry_point,
                                                     out);
}

void cw_calc_result_destroy(cw_calc_result *const calc_result) {
  hash_to_direct_approvers_entry_t *curr_tx_approver_entry = NULL;
  hash_to_direct_approvers_entry_t *tmp_tx_approver_entry = NULL;
  hash_entry_t *curr_direct_approver = NULL;
  hash_entry_t *tmp_direct_approver = NULL;
  cw_entry_t *curr_cw_entry = NULL;
  cw_entry_t *tmp_cw_entry = NULL;
  // Clean up txToApproverMap
  HASH_ITER(hh, calc_result->tx_to_approvers, curr_tx_approver_entry,
            tmp_tx_approver_entry) {
    HASH_ITER(hh, calc_result->tx_to_approvers->approvers, curr_direct_approver,
              tmp_direct_approver) {
      HASH_DEL(calc_result->tx_to_approvers->approvers, curr_direct_approver);
      free(curr_direct_approver);
    }
    HASH_DEL(calc_result->tx_to_approvers, curr_tx_approver_entry);
    free(curr_tx_approver_entry);
  }

  // Cleanup CWRatings Map
  HASH_ITER(hh, calc_result->cw_ratings, curr_cw_entry, tmp_cw_entry) {
    HASH_DEL(calc_result->cw_ratings, curr_cw_entry);
    free(curr_cw_entry);
  }
}
