/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/storage/sql/mariadb/connection.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

#define MARIADB_LOGGER_ID "mariadb"

static logger_id_t logger_id;

retcode_t storage_connection_init(storage_connection_t* const connection,
                                  storage_connection_config_t const* const config,
                                  storage_connection_type_t const type) {
  retcode_t ret = RC_OK;
  MYSQL* db = NULL;
  char* database = NULL;
  UNUSED(config);

  if (connection == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(MARIADB_LOGGER_ID, LOGGER_DEBUG, true);

  if (type == STORAGE_CONNECTION_TANGLE) {
    if ((connection->actual = calloc(1, sizeof(mariadb_tangle_connection_t))) == NULL) {
      return RC_OOM;
    }
    db = &((mariadb_tangle_connection_t*)connection->actual)->db;
    database = "tangle";
  } else if (type == STORAGE_CONNECTION_SPENT_ADDRESSES) {
    if ((connection->actual = calloc(1, sizeof(mariadb_spent_addresses_connection_t))) == NULL) {
      return RC_OOM;
    }
    db = &((mariadb_spent_addresses_connection_t*)connection->actual)->db;
    database = "spent-addresses";
  }
  connection->type = type;

  if (mysql_init(db) == NULL) {
    return RC_STORAGE_FAILED_INIT;
  }

  if (mysql_real_connect(db, "localhost", "ciri", NULL, database, 0, NULL, 0) == 0) {
    log_error(logger_id, "Connection to database %s failed: %s\n", database, mysql_error(db));
    return RC_STORAGE_FAILED_OPEN_DB;
  }

  return ret;
}

retcode_t storage_connection_destroy(storage_connection_t* const connection) {
  retcode_t ret = RC_OK;
  MYSQL* db = NULL;

  if (connection == NULL) {
    return RC_NULL_PARAM;
  }

  if (connection->type == STORAGE_CONNECTION_TANGLE) {
    db = &((mariadb_tangle_connection_t*)connection->actual)->db;
  } else if (connection->type == STORAGE_CONNECTION_SPENT_ADDRESSES) {
    db = &((mariadb_spent_addresses_connection_t*)connection->actual)->db;
  }

  mysql_close(db);

  free(connection->actual);

  return ret;
}
