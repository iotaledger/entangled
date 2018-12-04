/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/errors.h"
#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"
#include "utils/logger_helper.h"

retcode_t iota_consensus_cw_rating_init(cw_rating_calculator_t *const cw_calc,
                                        tangle_t *const tangle,
                                        cw_calculation_implementation_t impl) {
  logger_helper_init(CW_RATING_CALCULATOR_LOGGER_ID, LOGGER_DEBUG, true);
  cw_calc->tangle = tangle;
  if (impl == DFS_FROM_ENTRY_POINT) {
    init_cw_calculator_dfs(&cw_calc->base);
    return RC_OK;
  }
  return RC_OK;
}

retcode_t iota_consensus_cw_rating_destroy(cw_rating_calculator_t *cw_calc) {
  logger_helper_destroy(CW_RATING_CALCULATOR_LOGGER_ID);
  cw_calc->tangle = NULL;
  return RC_OK;
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
  hash_to_indexed_hash_set_map_free(&calc_result->tx_to_approvers);
  hash_int_map_free(&calc_result->cw_ratings);
}
