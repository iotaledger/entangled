#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "common/errors.h"
#include "consensus/cw_rating_calculator/cw_rating_dfs_impl.h"

retcode_t iota_consensus_cw_rating_init(cw_rating_calculator_t *cw_calc,
                                        const tangle_t *tangle,
                                        enum CWCalculationImplementation impl) {
  logger_helper_init(CW_RATING_CALCULATOR_LOGGER_ID, LOGGER_INFO, true);
  cw_calc->tangle = tangle;
  if (impl == NaiveDFS) {
    init_cw_calculator_dfs(&cw_calc->base);
  } else if (impl == NoImplementation) {
  }
}

retcode_t iota_consensus_cw_rating_calculate(
    const cw_rating_calculator_t *calculator, trit_array_p entry_point,
    cw_calc_result *out) {
  return calculator->base.vtable.cw_rating_calculate(calculator, entry_point,
                                                     out);
}