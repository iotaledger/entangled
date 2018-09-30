/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "utils/logger_helper.h"

#define WALKER_VALIDATOR_LOGGER_ID "consensus_walker_validator"

retcode_t iota_consensus_exit_prob_transaction_validator_init(
    tangle_t *const tangle, milestone_tracker_t *const mt,
    ledger_validator_t *const lv, exit_prob_transaction_validator_t *epv) {
  logger_helper_init(WALKER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  epv->tangle = tangle;
  epv->mt = mt;
  epv->lv = lv;
  return RC_OK;
}

retcode_t iota_consensus_exit_prob_transaction_validator_destroy(
    exit_prob_transaction_validator_t *epv) {
  logger_helper_destroy(WALKER_VALIDATOR_LOGGER_ID);
  epv->tangle = NULL;
  epv->mt = NULL;
  epv->lv = NULL;
  return RC_OK;
}
retcode_t iota_consensus_exit_prob_transaction_validator_is_valid(
    const exit_prob_transaction_validator_t *ep_validator, trit_array_p tx_hash,
    bool *is_valid) {
  retcode_t ret = RC_OK;

  struct _iota_transaction tx;
  iota_transaction_t tx_models = &tx;

  iota_stor_pack_t tx_pack = {(void **)(&tx_models), 1, 0, false};

  ret = iota_tangle_transaction_load(ep_validator->tangle, TRANSACTION_COL_HASH,
                                     tx_hash, &tx_pack);
  if (ret != RC_OK || tx_pack.num_loaded == 0) {
    return ret;
  }

  // TODO - check

  /*TransactionViewModel transactionViewModel =
  TransactionViewModel.fromHash(tangle, transactionHash); if
  (transactionViewModel.getType() == TransactionViewModel.PREFILLED_SLOT) {
    log.debug("Validation failed: {} is missing in db", transactionHash);
    return false;
  } else if (transactionViewModel.getCurrentIndex() != 0) {
    log.debug("Validation failed: {} not a tail", transactionHash);
    return false;
  } else if (!transactionViewModel.isSolid()) {
    log.debug("Validation failed: {} is not solid", transactionHash);
    return false;
  } else if (belowMaxDepth(transactionViewModel.getHash(),
  milestone.latestSolidSubtangleMilestoneIndex - maxDepth)) {
    log.debug("Validation failed: {} is below max depth", transactionHash);
    return false;
  } else if (!ledgerValidator.updateDiff(myApprovedHashes, myDiff,
  transactionViewModel.getHash())) { log.debug("Validation failed: {} is not
  consistent", transactionHash); return false;
  }
  return true;*/
  if (tx.current_index != 0) {
    log_error(WALKER_VALIDATOR_LOGGER_ID,
              "Validation failed, transaction is not a tail\n");
    return RC_OK;
  }

  *is_valid = true;

  return ret;
}

retcode_t iota_consensus_exit_prob_transaction_validator_below_max_depth(
    const exit_prob_transaction_validator_t *wv, trit_array_p tip,
    size_t depth) {
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}
