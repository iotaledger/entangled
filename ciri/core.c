/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/core.h"
#include "utils/logger_helper.h"

#define CORE_LOGGER_ID "core"

static logger_id_t logger_id;

retcode_t core_init(core_t* const core, tangle_t* const tangle) {
  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  logger_id = logger_helper_enable(CORE_LOGGER_ID, LOGGER_DEBUG, true);
  core->running = false;

  log_info(logger_id, "Initializing consensus\n");
  if (iota_consensus_init(&core->consensus, tangle, &core->node.transaction_requester, &core->node.tips) != RC_OK) {
    log_critical(logger_id, "Initializing consensus failed\n");
    return RC_CORE_FAILED_CONSENSUS_INIT;
  }

  log_info(logger_id, "Initializing node gossip components\n");
  if (node_init(&core->node, core, tangle) != RC_OK) {
    log_critical(logger_id, "Initializing node gossip components failed\n");
    return RC_CORE_FAILED_NODE_INIT;
  }

  return RC_OK;
}

retcode_t core_start(core_t* const core, tangle_t* const tangle) {
  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  log_info(logger_id, "Starting consensus\n");
  if (iota_consensus_start(&core->consensus, tangle) != RC_OK) {
    log_critical(logger_id, "Starting consensus failed\n");
    return RC_CORE_FAILED_CONSENSUS_START;
  }

  log_info(logger_id, "Starting node gossip components\n");
  if (node_start(&core->node) != RC_OK) {
    log_critical(logger_id, "Starting node gossip components failed\n");
    return RC_CORE_FAILED_NODE_START;
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

  log_info(logger_id, "Stopping node gossip components\n");
  if (node_stop(&core->node) != RC_OK) {
    log_error(logger_id, "Stopping node gossip components failed\n");
    ret = RC_CORE_FAILED_NODE_STOP;
  }

  log_info(logger_id, "Stopping consensus\n");
  if (iota_consensus_stop(&core->consensus) != RC_OK) {
    log_critical(logger_id, "Stopping consensus failed\n");
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

  log_info(logger_id, "Destroying node gossip components\n");
  if (node_destroy(&core->node) != RC_OK) {
    log_error(logger_id, "Destroying node gossip components failed\n");
    ret = RC_CORE_FAILED_NODE_DESTROY;
  }

  log_info(logger_id, "Destroying consensus\n");
  if (iota_consensus_destroy(&core->consensus) != RC_OK) {
    log_critical(logger_id, "Destroying consensus failed\n");
    ret = RC_CORE_FAILED_CONSENSUS_DESTROY;
  }

  logger_helper_release(logger_id);
  return ret;
}
