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

#define CORE_LOGGER_ID "core"

retcode_t core_init(core_t* const core) {
  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  logger_helper_init(CORE_LOGGER_ID, LOGGER_DEBUG, true);
  memset(core, 0, sizeof(core_t));
  core->running = false;

  log_info(CORE_LOGGER_ID, "Initializing database connection\n");
  core->db_conf.db_path = CIRI_DB_PATH;
  core->db_conf.index_address = true;
  core->db_conf.index_approvee = true;
  core->db_conf.index_bundle = true;
  core->db_conf.index_tag = true;
  core->db_conf.index_hash = true;
  if (init_connection(&core->db_conn, &core->db_conf)) {
    log_critical(CORE_LOGGER_ID, "Initializing database connection failed\n");
    return RC_CORE_FAILED_DATABASE_INIT;
  }

  log_info(CORE_LOGGER_ID, "Initializing cIRI node\n");
  if (node_init(&core->node, core)) {
    log_critical(CORE_LOGGER_ID, "Initializing cIRI node failed\n");
    return RC_CORE_FAILED_NODE_INIT;
  }

  return RC_OK;
}

retcode_t core_start(core_t* const core) {
  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  log_info(CORE_LOGGER_ID, "Starting cIRI node\n");
  if (node_start(&core->node)) {
    log_critical(CORE_LOGGER_ID, "Starting cIRI node failed\n");
    return RC_CORE_FAILED_NODE_START;
  }

  core->running = true;

  return RC_OK;
}

retcode_t core_stop(core_t* const core) {
  retcode_t ret = RC_OK;

  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  log_info(CORE_LOGGER_ID, "Stopping cIRI node\n");
  if (node_stop(&core->node)) {
    log_error(CORE_LOGGER_ID, "Stopping cIRI node failed\n");
    ret = RC_CORE_FAILED_NODE_STOP;
  }

  core->running = false;

  return ret;
}

retcode_t core_destroy(core_t* const core) {
  retcode_t ret = RC_OK;

  if (core == NULL) {
    return RC_CORE_NULL_CORE;
  }

  if (core->running) {
    return RC_CORE_STILL_RUNNING;
  }

  log_info(CORE_LOGGER_ID, "Destroying cIRI node\n");
  if (node_destroy(&core->node)) {
    log_error(CORE_LOGGER_ID, "Destroying cIRI node failed\n");
    ret = RC_CORE_FAILED_NODE_DESTROY;
  }

  log_info(CORE_LOGGER_ID, "Destroying database connection\n");
  if (destroy_connection(&core->db_conn)) {
    log_error(CORE_LOGGER_ID, "Destroying database connection failed\n");
    ret = RC_CORE_FAILED_DATABASE_DESTROY;
  }

  logger_helper_destroy(CORE_LOGGER_ID);
  return ret;
}
