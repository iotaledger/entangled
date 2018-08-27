/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/walker_validator/walker_validator.h"

#define WALKER_VALIDATOR_LOGGER_ID "consensus_walker_validator"

retcode_t iota_consensus_walker_validator_init(tangle_t *const tangle,
                                               milestone_t *const milestone,
                                               ledger_validator_t *const lv,
                                               walker_validator_t *const wv) {
  logger_helper_init(WALKER_VALIDATOR_LOGGER_ID, LOGGER_INFO, true);
  wv->tangle = tangle;
  wv->milestone = milestone;
  wv->lv = lv;
  return RC_OK;
}

retcode_t iota_consensus_walker_validator_destroy(
    walker_validator_t *const wv) {
  logger_helper_destroy(WALKER_VALIDATOR_LOGGER_ID);
  wv->tangle = NULL;
  wv->milestone = NULL;
  wv->lv = NULL;
  return RC_OK;
}

retcode_t iota_consensus_walker_validator_is_valid(const walker_validator_t *wv,
                                                   trit_array_p tx_hash) {
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}

retcode_t iota_consensus_walker_validator_below_max_depth(
    const walker_validator_t *wv, trit_array_p tip, size_t depth) {
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}
