/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/errors.h"
#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"
#include "utils/logger_helper.h"

#define CW_RATING_CALCULATOR_LOGGER_ID "cw_rating_calculator"

static logger_id_t logger_id;

retcode_t iota_consensus_cw_rating_init(cw_rating_calculator_t *const cw_calc, cw_calculation_implementation_t impl) {
  logger_id = logger_helper_enable(CW_RATING_CALCULATOR_LOGGER_ID, LOGGER_DEBUG, true);
  if (impl == DFS_FROM_ENTRY_POINT) {
    init_cw_calculator_dfs(&cw_calc->base);
    return RC_OK;
  }
  return RC_OK;
}

retcode_t iota_consensus_cw_rating_destroy(cw_rating_calculator_t *cw_calc) {
  logger_helper_release(logger_id);
  return RC_OK;
}

retcode_t iota_consensus_cw_rating_calculate(cw_rating_calculator_t const *const calculator, tangle_t *const tangle,
                                             flex_trit_t const *entry_point, cw_calc_result *out) {
  if (calculator->base.vtable.cw_rating_calculate == NULL) {
    log_error(logger_id, "Vtable is not initialized\n");
    return RC_CONSENSUS_NULL_PTR;
  }
  return calculator->base.vtable.cw_rating_calculate(calculator, tangle, entry_point, out);
}

void cw_calc_result_destroy(cw_calc_result *const calc_result) {
  hash_to_indexed_hash_set_map_free(&calc_result->tx_to_approvers);
  hash_to_int64_t_map_free(&calc_result->cw_ratings);
}
