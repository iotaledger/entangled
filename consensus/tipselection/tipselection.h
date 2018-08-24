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
#include "consensus/ledger_validator/ledger_validator.h"
#include "consensus/milestone/milestone.h"
#include "consensus/model.h"
#include "consensus/tangle/tangle.h"
#include "consensus/walker/walker.h"
#include "consensus/walker_validator/walker_validator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tipselection_t {
  tangle_t *tangle;
  entry_point_selector_t *ep_selector;
  cw_rating_calculator_t *cw_calc;
  milestone_t *milestone;
  ledger_validator_t *lv;
  walker_validator_t *wv;
  walker_t *walker;
} tipselection_t;

extern retcode_t iota_consensus_tipselection_init(
    tipselection_t *impl, const tangle_t *tangle, const ledger_validator_t *lv,
    const walker_validator_t *wv, const cw_rating_calculator_t *cw_calc,
    const milestone_t *milestone, const entry_point_selector_t *ep,
    const walker_t *walker);

extern retcode_t iota_consensus_get_transactions_to_approve(
    tipselection_t *impl, size_t depth, const trit_array_p reference,
    tips_pair *tips);

extern retcode_t iota_consensus_tipselection_destroy(tipselection_t *impl);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TIPSELECTION_TIPSELECTION_H__
