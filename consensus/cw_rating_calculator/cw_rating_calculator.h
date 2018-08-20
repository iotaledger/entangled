/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CONCENSUS_CW_RATING_CALCULATOR_CW_RATING_CALCULATOR_H__
#define __COMMON_CONCENSUS_CW_RATING_CALCULATOR_CW_RATING_CALCULATOR_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/trinary/trit_array.h"
#include "consensus/tangle/tangle.h"
#include "uthash.h"

#ifdef __cplusplus
extern "C" {
#endif

enum CWCalculationImplementation {
  NoImplementation,
  NaiveDFS,
};

typedef struct cw_entry_t {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  int64_t cw;
  UT_hash_handle hh;
} cw_entry_t;
typedef cw_entry_t *cw_map_t;

typedef struct hash_entry_t {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  UT_hash_handle hh;
} hash_entry_t;
typedef hash_entry_t *hash_set_t;

typedef struct hash_to_direct_approvers_entry_t {
  flex_trit_t hash[FLEX_TRIT_SIZE_243]; /*key*/
  uint64_t idx;
  hash_set_t approvers; /*value*/
  UT_hash_handle hh;
} hash_to_direct_approvers_entry_t;

typedef hash_to_direct_approvers_entry_t *hash_to_direct_approvers_map_t;

typedef struct cw_calc_result {
  cw_map_t cw_ratings;
  hash_to_direct_approvers_map_t tx_to_approvers;
} cw_calc_result;

typedef struct cw_rating_calculator_base cw_rating_calculator_base_t;
typedef struct cw_rating_calculator_t cw_rating_calculator_t;

typedef struct {
  // find_transactions_request
  retcode_t (*cw_rating_calculate)(const cw_rating_calculator_t *const,
                                   trit_array_p entry_point,
                                   cw_calc_result *result);

} cw_calculator_vtable;

struct cw_rating_calculator_base {
  cw_calculator_vtable vtable;
};

struct cw_rating_calculator_t {
  tangle_t *tangle;
  cw_rating_calculator_base_t base;
};

extern retcode_t iota_consensus_cw_rating_init(
    cw_rating_calculator_t *cw_calc, const tangle_t *tangle,
    enum CWCalculationImplementation impl);

extern retcode_t iota_consensus_cw_rating_calculate(
    const cw_rating_calculator_t *cw_calc, trit_array_p entry_point,
    cw_calc_result *out);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CONCENSUS_CW_RATING_CALCULATOR_CW_RATING_CALCULATOR_H__
