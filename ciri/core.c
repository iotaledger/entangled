/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/core.h"
#include "utils/logger_helper.h"

// TODO(thibault) from configuration variables
#define CIRI_DB_PATH "ciri/ciri.db"
#define CORE_LOGGER_ID "core"

bool core_init(core_t* const core) {
  if (core == NULL) {
    return false;
  }
  logger_helper_init(CORE_LOGGER_ID, LOGGER_DEBUG, true);
  memset(core, 0, sizeof(core_t));

  log_info(CORE_LOGGER_ID, "Initializing database connection\n");
  core->db_conf.db_path = CIRI_DB_PATH;
  core->db_conf.index_address = true;
  core->db_conf.index_approvee = true;
  core->db_conf.index_bundle = true;
  core->db_conf.index_tag = true;
  if (init_connection(&core->db_conn, &core->db_conf) != RC_OK) {
    log_critical(CORE_LOGGER_ID, "Initializing database connection failed\n");
    return false;
  }
  log_info(CORE_LOGGER_ID, "Initializing cIRI node\n");
  if (node_init(&core->node) == false) {
    log_critical(CORE_LOGGER_ID, "Initializing cIRI node failed\n");
    return false;
  }
  return true;
}

bool core_start(core_t* const core) {
  if (core == NULL) {
    return false;
  }
  log_info(CORE_LOGGER_ID, "Starting cIRI node\n");
  if (node_start(&core->node) == false) {
    log_critical(CORE_LOGGER_ID, "Starting cIRI node failed\n");
    return false;
  }
  return true;
}

bool core_stop(core_t* const core) {
  bool ret = true;

  if (core == NULL) {
    return false;
  }
  log_info(CORE_LOGGER_ID, "Stopping cIRI node\n");
  if (node_stop(&core->node) == false) {
    log_error(CORE_LOGGER_ID, "Stopping cIRI node failed\n");
    ret = false;
  }
  return ret;
}

bool core_destroy(core_t* const core) {
  bool ret = true;

  if (core == NULL) {
    return false;
  }
  log_info(CORE_LOGGER_ID, "Destroying cIRI node\n");
  if (node_destroy(&core->node) == false) {
    log_error(CORE_LOGGER_ID, "Destroying cIRI node failed\n");
    ret = false;
  }
  log_info(CORE_LOGGER_ID, "Destroying database connection\n");
  if (destroy_connection(&core->db_conn) != RC_OK) {
    log_error(CORE_LOGGER_ID, "Destroying database connection failed\n");
    ret = false;
  }
  logger_helper_destroy(CORE_LOGGER_ID);
  return ret;
}
