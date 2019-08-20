/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <mysql.h>

#include "common/storage/storage.h"
#include "utils/logger_helper.h"

#define MARIADB_LOGGER_ID "mariadb"

static logger_id_t logger_id;

retcode_t storage_init() {
  logger_id = logger_helper_enable(MARIADB_LOGGER_ID, LOGGER_DEBUG, true);

  if (mysql_library_init(0, NULL, NULL) != 0) {
    return RC_STORAGE_FAILED_INIT;
  }

  return RC_OK;
}

retcode_t storage_destroy() {
  logger_helper_release(logger_id);

  mysql_library_end();

  return RC_OK;
}
