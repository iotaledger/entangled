/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/core.h"
#include "utils/logger_helper.h"

// FIXME: Get cIRI database path from configuration variables
// https://github.com/iotaledger/entangled/issues/132
#define CIRI_DB_PATH "ciri/ciri.db"
#define CORE_LOGGER_ID "core"

retcode_t core_init(core_t* const core) {
  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  logger_helper_init(CORE_LOGGER_ID, LOGGER_DEBUG, true);
  core->running = false;

  connection_config_t db_conf = {.db_path = CIRI_DB_PATH};

  log_info(CORE_LOGGER_ID, "Initializing consensus\n");
  if (iota_consensus_init(&core->consensus, &db_conf,
                          &core->node.transaction_requester) != RC_OK) {
    log_critical(CORE_LOGGER_ID, "Initializing consensus failed\n");
    return RC_CORE_FAILED_CONSENSUS_INIT;
  }

  log_info(CORE_LOGGER_ID, "Initializing gossiping node\n");
  if (node_init(&core->node, core, &core->consensus.tangle)) {
    log_critical(CORE_LOGGER_ID, "Initializing gossiping node failed\n");
    return RC_CORE_FAILED_NODE_INIT;
  }

  log_info(CORE_LOGGER_ID, "Initializing API\n");
  if (iota_api_init(&core->api, &core->consensus.tangle,
                    &core->consensus.transaction_validator,
                    &core->node.broadcaster, SR_JSON)) {
    log_critical(CORE_LOGGER_ID, "Initializing API failed\n");
    return RC_CORE_FAILED_API_INIT;
  }

  return RC_OK;
}

retcode_t core_start(core_t* const core) {
  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  log_info(CORE_LOGGER_ID, "Starting consensus\n");
  if (iota_consensus_start(&core->consensus) != RC_OK) {
    log_critical(CORE_LOGGER_ID, "Starting consensus failed\n");
    return RC_CORE_FAILED_CONSENSUS_START;
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

  log_info(CORE_LOGGER_ID, "Stopping consensus\n");
  if (iota_consensus_stop(&core->consensus) != RC_OK) {
    log_critical(CORE_LOGGER_ID, "Stopping consensus failed\n");
    return RC_CORE_FAILED_CONSENSUS_STOP;
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

  log_info(CORE_LOGGER_ID, "Destroying cIRI node\n");
  if (node_destroy(&core->node)) {
    log_error(CORE_LOGGER_ID, "Destroying cIRI node failed\n");
    ret = RC_CORE_FAILED_NODE_DESTROY;
  }

  log_info(CORE_LOGGER_ID, "Destroying consensus\n");
  if (iota_consensus_stop(&core->consensus) != RC_OK) {
    log_critical(CORE_LOGGER_ID, "Destroying consensus failed\n");
    return RC_CORE_FAILED_CONSENSUS_DESTROY;
  }

  logger_helper_destroy(CORE_LOGGER_ID);
  return ret;
}
