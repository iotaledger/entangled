/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TIPSELECTION_TIPSELECTION_H__
#define __CONSENSUS_TIPSELECTION_TIPSELECTION_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/trinary/trit_array.h"
#include "consensus/cw_rating_calculator/cw_rating_calculator.h"
#include "consensus/entry_point_selector/entry_point_selector.h"
#include "consensus/exit_probability_randomizer/exit_probability_randomizer.h"
#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone/milestone.h"
#include "consensus/model.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tipselection_t {
  cw_rating_calculator_t *cw_calc;
  entry_point_selector_t *ep_selector;
  ledger_validator_t *lv;
  milestone_t *milestone;
  tangle_t *tangle;
  exit_prob_transaction_validator_t *wv;
  ep_randomizer_t *ep_randomizer;
} tipselection_t;

extern retcode_t iota_consensus_tipselection_init(
    tipselection_t *impl, const tangle_t *tangle, const ledger_validator_t *lv,
    const exit_prob_transaction_validator_t *wv,
    const cw_rating_calculator_t *cw_calc, const milestone_t *milestone,
    const entry_point_selector_t *ep, const ep_randomizer_t *ep_randomizer,
    double alpha);

extern retcode_t iota_consensus_get_transactions_to_approve(
    tipselection_t *const ts, size_t const depth, trit_array_p const reference,
    tips_pair *const tips);

extern retcode_t iota_consensus_tipselection_destroy(tipselection_t *const ts);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TIPSELECTION_TIPSELECTION_H__
