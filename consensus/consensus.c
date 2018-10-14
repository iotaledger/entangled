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

// FIXME: waiting for a stable location of these files
#define CIRI_SNAPSHOT_FILE "ciri/snapshotTestnet.txt"
#define CIRI_SNAPSHOT_SIG_FILE "ciri/snapshotTestnet.sig"

retcode_t iota_consensus_init(iota_consensus_t *const consensus,
                              connection_config_t const *const db_conf,
                              requester_state_t *const transaction_requester,
                              bool testnet) {
  retcode_t ret = RC_OK;

  logger_helper_init(CONSENSUS_LOGGER_ID, LOGGER_DEBUG, true);

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
           &consensus->tangle, &consensus->milestone_tracker,
           &consensus->ledger_validator,
           &consensus->exit_prob_transaction_validator, MAX_ANALYZED_TXS,
           MAX_DEPTH)) != RC_OK) {
    log_critical(
        CONSENSUS_LOGGER_ID,
        "Initializing exit probability transaction validator failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing milestone tracker\n");
  if ((ret = iota_milestone_tracker_init(
           &consensus->milestone_tracker, &consensus->tangle,
           &consensus->snapshot, &consensus->ledger_validator, testnet)) !=
      RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Initializing milestone tracker failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing snapshot\n");
  if ((ret = iota_snapshot_init(&consensus->snapshot, CIRI_SNAPSHOT_FILE,
                                CIRI_SNAPSHOT_SIG_FILE, testnet)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Initializing snapshot failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing tangle\n");
  if ((ret = iota_tangle_init(&consensus->tangle, db_conf)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Initializing tangle failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing tip selection\n");
  if ((ret = iota_consensus_tipselection_init(
           &consensus->tipselection, &consensus->tangle,
           &consensus->ledger_validator,
           &consensus->exit_prob_transaction_validator,
           &consensus->cw_rating_calculator, &consensus->milestone_tracker,
           &consensus->entry_point_selector, &consensus->ep_randomizer, ALPHA,
           MAX_ANALYZED_TXS, MAX_DEPTH)) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID, "Initializing tip selection failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing transaction validator\n");
  if ((ret = iota_consensus_transaction_validator_init(
           &consensus->transaction_validator, SNAPSHOT_TIMESTAMP * 1000,
           (testnet ? TESTNET_MWM : MAINNET_MWM))) != RC_OK) {
    log_critical(CONSENSUS_LOGGER_ID,
                 "Initializing transaction validator failed\n");
    return ret;
  }

  log_info(CONSENSUS_LOGGER_ID, "Initializing ledger validator\n");
  if ((ret = iota_consensus_ledger_validator_init(
           &consensus->ledger_validator, &consensus->tangle,
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

  log_info(CONSENSUS_LOGGER_ID, "Destroying snapshot\n");
  if ((ret = iota_snapshot_destroy(&consensus->snapshot)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying snapshot failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying tangle\n");
  if ((ret = iota_tangle_destroy(&consensus->tangle)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying tangle failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying tip selection\n");
  if ((ret = iota_consensus_tipselection_destroy(&consensus->tipselection))) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying tip selection failed\n");
  }

  log_info(CONSENSUS_LOGGER_ID, "Destroying transaction validator\n");
  if ((ret = iota_consensus_transaction_validator_destroy(
           &consensus->transaction_validator)) != RC_OK) {
    log_error(CONSENSUS_LOGGER_ID, "Destroying transaction validator failed\n");
  }

  logger_helper_destroy(CONSENSUS_LOGGER_ID);

  return ret;
}
