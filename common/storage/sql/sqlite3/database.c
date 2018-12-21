/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <sqlite3.h>

#include "common/storage/database.h"
#include "utils/logger_helper.h"

#define SQLITE3_LOGGER_ID "sqlite3"

static void error_log_callback(void* const arg, int const err_code,
                               char const* const message) {
  log_error(SQLITE3_LOGGER_ID, "Failed with error code %d: %s\n", err_code,
            message);
}

retcode_t database_init() {
  logger_helper_init(SQLITE3_LOGGER_ID, LOGGER_DEBUG, true);

  if (sqlite3_config(SQLITE_CONFIG_LOG, error_log_callback, NULL) !=
      SQLITE_OK) {
    return RC_SQLITE3_FAILED_CONFIG;
  }

  // TODO - implement connections pool so no two threads
  // will access db through same connection simultaneously
  if (sqlite3_config(SQLITE_CONFIG_SERIALIZED) != SQLITE_OK) {
    return RC_SQLITE3_FAILED_CONFIG;
  }

  if (sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 0) != SQLITE_OK) {
    return RC_SQLITE3_FAILED_CONFIG;
  }

  if (sqlite3_initialize() != SQLITE_OK) {
    return RC_SQLITE3_FAILED_INITIALIZE;
  }

  return RC_OK;
}

retcode_t database_destroy() {
  logger_helper_destroy(SQLITE3_LOGGER_ID);

  if (sqlite3_shutdown() != SQLITE_OK) {
    return RC_SQLITE3_FAILED_SHUTDOWN;
  }

  return RC_OK;
}
