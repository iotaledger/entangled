/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/core.h"
#include "utils/logger_helper.h"

// FIXME: Get cIRI database path from configuration variables
// https://github.com/iotaledger/entangled/issues/132
#define CIRI_DB_PATH "ciri/ciri.db"
// FIXME: waiting for a stable location of these files
#define CIRI_SNAPSHOT_FILE "ciri/snapshotTestnet.txt"
#define CIRI_SNAPSHOT_SIG_FILE "ciri/snapshotTestnet.sig"

#define CORE_LOGGER_ID "core"

retcode_t core_init(core_t* const core) {
  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  logger_helper_init(CORE_LOGGER_ID, LOGGER_DEBUG, true);
  core->running = false;

  log_info(CORE_LOGGER_ID, "Initializing snapshot\n");
  if (iota_snapshot_init(&core->snapshot, CIRI_SNAPSHOT_FILE,
                         CIRI_SNAPSHOT_SIG_FILE, core->config.testnet)) {
    log_critical(CORE_LOGGER_ID, "Initializing snapshot failed\n");
    return RC_CORE_FAILED_SNAPSHOT_INIT;
  }

  log_info(CORE_LOGGER_ID, "Initializing tangle\n");
  core->db_conf.db_path = CIRI_DB_PATH;
  core->db_conf.index_transaction_address = true;
  core->db_conf.index_transaction_approvee = true;
  core->db_conf.index_transaction_bundle = true;
  core->db_conf.index_transaction_tag = true;
  core->db_conf.index_transaction_hash = true;
  core->db_conf.index_milestone_hash = true;
  if (iota_tangle_init(&core->tangle, &core->db_conf)) {
    log_critical(CORE_LOGGER_ID, "Initializing tangle failed\n");
    return RC_CORE_FAILED_DATABASE_INIT;
  }

  log_info(CORE_LOGGER_ID, "Initializing milestone tracker\n");
  if (iota_milestone_tracker_init(&core->milestone_tracker, &core->tangle,
                                  &core->snapshot, core->config.testnet)) {
    log_critical(CORE_LOGGER_ID, "Initializing milestone tracker failed\n");
    return RC_CORE_FAILED_MILESTONE_TRACKER_INIT;
  }

  log_info(CORE_LOGGER_ID, "Initializing transaction requester\n");
  if (requester_init(&core->transaction_requester, &core->tangle)) {
    log_critical(CORE_LOGGER_ID, "Initializing transaction requester failed\n");
    return RC_CORE_FAILED_REQUESTER_INIT;
  }

  log_info(CORE_LOGGER_ID, "Initializing cIRI node\n");
  if (node_init(&core->node, core, &core->transaction_requester)) {
    log_critical(CORE_LOGGER_ID, "Initializing cIRI node failed\n");
    return RC_CORE_FAILED_NODE_INIT;
  }

  log_info(CORE_LOGGER_ID, "Initializing ledger validator\n");
  if (iota_consensus_ledger_validator_init(
          &core->ledger_validator, &core->tangle, &core->milestone_tracker,
          &core->transaction_requester)) {
    log_critical(CORE_LOGGER_ID, "Initializing ledger validator\n");
    return RC_CORE_FAILED_LEDGER_VALIDATOR_INIT;
  }

  log_info(CORE_LOGGER_ID, "Initializing cIRI API\n");
  if (iota_api_init(&core->api, core->config.api_port)) {
    log_critical(CORE_LOGGER_ID, "Initializing cIRI API failed\n");
    return RC_CORE_FAILED_API_INIT;
  }

  return RC_OK;
}

retcode_t core_start(core_t* const core) {
  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  log_info(CORE_LOGGER_ID, "Starting milestone tracker\n");
  if (iota_milestone_tracker_start(&core->milestone_tracker)) {
    log_critical(CORE_LOGGER_ID, "Starting milestone tracker failed\n");
    return RC_CORE_FAILED_MILESTONE_TRACKER_START;
  }

  log_info(CORE_LOGGER_ID, "Starting cIRI node\n");
  if (node_start(&core->node)) {
    log_critical(CORE_LOGGER_ID, "Starting cIRI node failed\n");
    return RC_CORE_FAILED_NODE_START;
  }

  log_info(CORE_LOGGER_ID, "Starting cIRI API\n");
  if (iota_api_start(&core->api)) {
    log_critical(CORE_LOGGER_ID, "Starting cIRI API failed\n");
    return RC_CORE_FAILED_API_START;
  }

  core->running = true;

  return RC_OK;
}

retcode_t core_stop(core_t* const core) {
  retcode_t ret = RC_OK;

  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  } else if (core->running == false) {
    return RC_OK;
  }

  core->running = false;

  log_info(CORE_LOGGER_ID, "Stopping milestone tracker\n");
  if (iota_milestone_tracker_stop(&core->milestone_tracker)) {
    log_critical(CORE_LOGGER_ID, "Stopping milestone tracker failed\n");
    return RC_CORE_FAILED_MILESTONE_TRACKER_STOP;
  }

  log_info(CORE_LOGGER_ID, "Stopping cIRI node\n");
  if (node_stop(&core->node)) {
    log_error(CORE_LOGGER_ID, "Stopping cIRI node failed\n");
    ret = RC_CORE_FAILED_NODE_STOP;
  }

  log_info(CORE_LOGGER_ID, "Stopping cIRI API\n");
  if (iota_api_stop(&core->api)) {
    log_error(CORE_LOGGER_ID, "Stopping cIRI API failed\n");
    ret = RC_CORE_FAILED_API_STOP;
  }

  return ret;
}

retcode_t core_destroy(core_t* const core) {
  retcode_t ret = RC_OK;

  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  } else if (core->running) {
    return RC_CORE_STILL_RUNNING;
  }

  log_info(CORE_LOGGER_ID, "Destroying cIRI API\n");
  if (iota_api_destroy(&core->api)) {
    log_error(CORE_LOGGER_ID, "Destroying cIRI API failed\n");
    ret = RC_CORE_FAILED_API_DESTROY;
  }

  log_info(CORE_LOGGER_ID, "Destroying ledger validator\n");
  if (iota_consensus_ledger_validator_destroy(&core->ledger_validator)) {
    log_error(CORE_LOGGER_ID, "Destroying ledger validator failed\n");
    ret = RC_CORE_FAILED_LEDGER_VALIDATOR_DESTROY;
  }

  log_info(CORE_LOGGER_ID, "Destroying cIRI node\n");
  if (node_destroy(&core->node)) {
    log_error(CORE_LOGGER_ID, "Destroying cIRI node failed\n");
    ret = RC_CORE_FAILED_NODE_DESTROY;
  }

  log_info(CORE_LOGGER_ID, "Destroying transaction requester\n");
  if (requester_destroy(&core->transaction_requester)) {
    log_error(CORE_LOGGER_ID, "Destroying transaction requester failed\n");
    ret = RC_CORE_FAILED_REQUESTER_DESTROY;
  }

  log_info(CORE_LOGGER_ID, "Destroying milestone tracker\n");
  if (iota_milestone_tracker_stop(&core->milestone_tracker)) {
    log_error(CORE_LOGGER_ID, "Destroying milestone tracker failed\n");
    ret = RC_CORE_FAILED_MILESTONE_TRACKER_DESTROY;
  }

  log_info(CORE_LOGGER_ID, "Destroying tangle\n");
  if (iota_tangle_destroy(&core->tangle)) {
    log_error(CORE_LOGGER_ID, "Destroying tangle failed\n");
    ret = RC_CORE_FAILED_DATABASE_DESTROY;
  }

  log_info(CORE_LOGGER_ID, "Destroying snapshot\n");
  if (iota_snapshot_destroy(&core->snapshot)) {
    log_error(CORE_LOGGER_ID, "Destroying snapshot failed\n");
    ret = RC_CORE_FAILED_SNAPSHOT_DESTROY;
  }

  logger_helper_destroy(CORE_LOGGER_ID);
  return ret;
}
