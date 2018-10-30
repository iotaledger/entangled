/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/consensus.h"
#include "ciri/conf/conf_values.h"
#include "utils/logger_helper.h"

#define CONSENSUS_LOGGER_ID "consensus"

retcode_t iota_consensus_init(iota_consensus_t *const consensus,
                              connection_config_t const *const db_conf,
                              requester_state_t *const transaction_requester,
                              bool testnet) {
  retcode_t ret = RC_OK;

  logger_helper_init(CONSENSUS_LOGGER_ID, LOGGER_DEBUG, true);
  memset(consensus->defs.genesis_hash, FLEX_TRIT_NULL_VALUE,
         FLEX_TRIT_SIZE_243);

  log_info(CONSENSUS_LOGGER_ID, "Initializing bundle validator\n");
  if ((ret = iota_consensus_bundle_validator_init()) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Initializing bundle validator failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID,
           "Initializing cumulative weight rating calculator\n");
  if ((ret = iota_consensus_cw_rating_init(&consensus->cw_rating_calculator,
                                           &consensus->tangle, CW_CALC_IMPL)) !=
      RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Initializing cumulative weight rating calculator failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing entry point selector\n");
  if ((ret = iota_consensus_entry_point_selector_init(
           &consensus->entry_point_selector, &consensus->milestone_tracker,
           &consensus->tangle, testnet)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Initializing entry point selector failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing exit probability randomizer\n");
  if ((ret = iota_consensus_ep_randomizer_init(&consensus->ep_randomizer,
                                               &consensus->tangle, ALPHA,
                                               EP_RAND_IMPL)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Initializing exit probability randomizer failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID,
           "Initializing exit probability transaction validator\n");
  if ((ret = iota_consensus_exit_prob_transaction_validator_init(
           &consensus->defs, &consensus->tangle, &consensus->milestone_tracker,
           &consensus->ledger_validator,
           &consensus->exit_prob_transaction_validator, MAX_ANALYZED_TXS,
           MAX_DEPTH)) != RC_OK) {
    log_critical(
        CONSENSUS_LOGGER_ID,
        "Initializing exit probability transaction validator failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing snapshot\n");
  if ((ret = iota_snapshot_init(
           &consensus->snapshot,
           (testnet ? SNAPSHOT_TESTNET : SNAPSHOT_MAINNET),
           (testnet ? NULL : SNAPSHOT_SIG_MAINNET),
           (testnet ? SNAPSHOT_CONF_TESTNET : SNAPSHOT_CONF_MAINNET),
           testnet)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Initializing snapshot failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing transaction solidifier\n");
  if ((ret = iota_consensus_transaction_solidifier_init(
           &consensus->transaction_solidifier, &consensus->defs,
           &consensus->tangle, transaction_requester)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Initializing transaction solidifier failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing milestone tracker\n");
  if ((ret = iota_milestone_tracker_init(
           &consensus->milestone_tracker, &consensus->tangle,
           &consensus->snapshot, &consensus->ledger_validator,
           &consensus->transaction_solidifier, testnet)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Initializing milestone tracker failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing tangle\n");
  if ((ret = iota_tangle_init(&consensus->tangle, db_conf)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Initializing tangle failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing tip selector\n");
  if ((ret = iota_consensus_tip_selector_init(
           &consensus->tip_selector, &consensus->cw_rating_calculator,
           &consensus->entry_point_selector, &consensus->ep_randomizer,
           &consensus->exit_prob_transaction_validator,
           &consensus->ledger_validator, &consensus->milestone_tracker,
           &consensus->tangle, ALPHA, MAX_ANALYZED_TXS, MAX_DEPTH)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Initializing tip selector failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing transaction validator\n");
  if ((ret = iota_consensus_transaction_validator_init(
           &consensus->transaction_validator, &consensus->defs,
           consensus->snapshot.conf.timestamp_sec * 1000UL,
           (testnet ? TESTNET_MWM : MAINNET_MWM))) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Initializing transaction validator failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing ledger validator\n");
  if ((ret = iota_consensus_ledger_validator_init(
           &consensus->ledger_validator, &consensus->defs, &consensus->tangle,
           &consensus->milestone_tracker, transaction_requester)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Initializing ledger validator failed\n");
    return ret;
  }

  return ret;
}

retcode_t iota_consensus_start(iota_consensus_t *const consensus) {
  retcode_t ret = RC_OK;

  log_info(CONSENSUS_LOGGER_ID, "Starting milestone tracker\n");
  if ((ret = iota_milestone_tracker_start(&consensus->milestone_tracker)) !=
      RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Starting milestone tracker failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Starting transaction_solidifier\n");
  if ((ret = iota_consensus_transaction_solidifier_start(
           &consensus->transaction_solidifier)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Starting transaction_solidifier failed\n");
    return ret;
  }

  return ret;
}

retcode_t iota_consensus_stop(iota_consensus_t *const consensus) {
  retcode_t ret = RC_OK;

  log_info(CONSENSUS_LOGGER_ID, "Stopping milestone tracker\n");
  if ((ret = iota_milestone_tracker_start(&consensus->milestone_tracker)) !=
      RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Stopping milestone tracker failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Stopping transaction_solidifier\n");
  if ((ret = iota_consensus_transaction_solidifier_stop(
           &consensus->transaction_solidifier)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Stopping transaction_solidifier failed\n");
    return ret;
  }

  return ret;
}

retcode_t iota_consensus_destroy(iota_consensus_t *const consensus) {
  retcode_t ret = RC_OK;

  log_info(CONSENSUS_LOGGER_ID, "Destroying bundle validator\n");
  if ((ret = iota_consensus_bundle_validator_destroy()) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying bundle validator failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID,
           "Destroying cumulative weight rating calculator\n");
  if ((ret = iota_consensus_cw_rating_destroy(
           &consensus->cw_rating_calculator)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID,
              "Destroying cumulative weight rating calculator failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying entry point selector\n");
  if ((ret = iota_consensus_entry_point_selector_destroy(
           &consensus->entry_point_selector)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying entry point selector failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying exit probability randomizer\n");
  if ((ret = iota_consensus_ep_randomizer_destroy(&consensus->ep_randomizer)) !=
      RC_OK) {
    log_error(CONSENSUS_LOGGER_ID,
              "Destroying exit probability randomizer failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID,
           "Destroying exit probability transaction validator\n");
  if ((ret = iota_consensus_exit_prob_transaction_validator_destroy(
           &consensus->exit_prob_transaction_validator))) {
    log_error(CONSENSUS_LOGGER_ID,
              "Destroying exit probability transaction validator failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying ledger validator\n");
  if ((ret = iota_consensus_ledger_validator_destroy(
           &consensus->ledger_validator)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying ledger validator failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying milestone tracker\n");
  if ((ret = iota_milestone_tracker_destroy(&consensus->milestone_tracker)) !=
      RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying milestone tracker failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying transaction solidifier\n");
  if ((ret = iota_consensus_transaction_solidifier_destroy(
           &consensus->transaction_solidifier)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID,
              "Destroying transaction solidifier failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying snapshot\n");
  if ((ret = iota_snapshot_destroy(&consensus->snapshot)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying snapshot failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying tangle\n");
  if ((ret = iota_tangle_destroy(&consensus->tangle)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying tangle failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying tip selector\n");
  if ((ret = iota_consensus_tip_selector_destroy(&consensus->tip_selector))) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying tip selector failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying transaction validator\n");
  if ((ret = iota_consensus_transaction_validator_destroy(
           &consensus->transaction_validator)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying transaction validator failed\n");
  }

  logger_helper_destroy(CONSENSUS_LOGGER_ID);

  return ret;
}
