/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_CW_RATING_CALCULATOR_CW_RATING_CALCULATOR_H__
#define __CONSENSUS_CW_RATING_CALCULATOR_CW_RATING_CALCULATOR_H__

#include <stdbool.h>
#include <stdint.h>

#include "uthash.h"

#include "ciri/consensus/tangle/tangle.h"
#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utils/containers/hash/hash_int64_t_map.h"
#include "utils/hash_indexed_map.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct cw_rating_calculator_base cw_rating_calculator_base_t;
typedef struct cw_rating_calculator_t cw_rating_calculator_t;

typedef enum cw_calculation_implementation_e {
  CW_NO_IMPLEMENTATION,
  /// time - O(n^2), place - O(n^2)
  DFS_FROM_ENTRY_POINT,
  /// time - O(n), place - O(n^2) implementation with the cost of
  /// Performing propogation on each incoming transaction
  BACKWARD_WEIGHT_PROPAGATION,
} cw_calculation_implementation_t;

typedef struct cw_calc_result {
  hash_to_int64_t_map_t cw_ratings;
  hash_to_indexed_hash_set_map_t tx_to_approvers;
} cw_calc_result;

typedef struct {
  // find_transactions_request
  retcode_t (*cw_rating_calculate)(cw_rating_calculator_t const *const, tangle_t *const tangle,
                                   flex_trit_t const *const entry_point, cw_calc_result *const result);

} cw_calculator_vtable;

struct cw_rating_calculator_base {
  cw_calculator_vtable vtable;
};

struct cw_rating_calculator_t {
  cw_rating_calculator_base_t base;
};

extern retcode_t iota_consensus_cw_rating_init(cw_rating_calculator_t *const cw_calc,
                                               cw_calculation_implementation_t impl);

extern retcode_t iota_consensus_cw_rating_destroy(cw_rating_calculator_t *cw_calc);

extern retcode_t iota_consensus_cw_rating_calculate(cw_rating_calculator_t const *const cw_calc, tangle_t *const tangle,
                                                    flex_trit_t const *const entry_point, cw_calc_result *const out);

extern void cw_calc_result_destroy(cw_calc_result *const calc_result);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_CW_RATING_CALCULATOR_CW_RATING_CALCULATOR_H__
