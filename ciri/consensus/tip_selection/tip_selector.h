/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TIP_SELECTOR_TIP_SELECTOR_H__
#define __CONSENSUS_TIP_SELECTOR_TIP_SELECTOR_H__

#include <stdbool.h>
#include <stdint.h>

#include "ciri/consensus/conf.h"
#include "ciri/consensus/ledger_validator/ledger_validator.h"
#include "ciri/consensus/milestone/milestone_tracker.h"
#include "ciri/consensus/model.h"
#include "ciri/consensus/tangle/tangle.h"
#include "ciri/consensus/tip_selection/cw_rating_calculator/cw_rating_calculator.h"
#include "ciri/consensus/tip_selection/entry_point_selector/entry_point_selector.h"
#include "ciri/consensus/tip_selection/exit_probability_randomizer/exit_probability_randomizer.h"
#include "ciri/consensus/tip_selection/exit_probability_validator/exit_probability_validator.h"
#include "common/errors.h"
#include "common/trinary/trit_array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tip_selector_s {
  iota_consensus_conf_t *conf;
  cw_rating_calculator_t *cw_rating_calculator;
  entry_point_selector_t *entry_point_selector;
  ep_randomizer_t *ep_randomizer;
  ledger_validator_t *ledger_validator;
  milestone_tracker_t *milestone_tracker;
} tip_selector_t;

retcode_t iota_consensus_tip_selector_init(tip_selector_t *const tip_selector, iota_consensus_conf_t *const conf,
                                           cw_rating_calculator_t *const cw_rating_calculator,
                                           entry_point_selector_t *const entry_point_selector,
                                           ep_randomizer_t *const ep_randomizer,
                                           ledger_validator_t *const ledger_validator,
                                           milestone_tracker_t *const milestone_tracker);

retcode_t iota_consensus_tip_selector_get_transactions_to_approve(tip_selector_t *const tip_selector,
                                                                  tangle_t *const tangle, size_t const depth,
                                                                  flex_trit_t const *const reference,
                                                                  tips_pair_t *const tips);

retcode_t iota_consensus_tip_selector_destroy(tip_selector_t *const tip_selector);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TIP_SELECTOR_TIP_SELECTOR_H__
