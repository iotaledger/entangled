/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/storage/test_utils.h"
#include "common/storage/defs.h"
#include "common/storage/sql/mariadb/connection.h"
#include "utils/macros.h"

retcode_t storage_test_setup(storage_connection_t* const connection, storage_connection_config_t const* const config,
                             char const* const test_db_path, storage_connection_type_t const type) {
  UNUSED(test_db_path);

  return storage_connection_init(connection, config, type);
}

retcode_t storage_test_teardown(storage_connection_t* const connection, char const* const test_db_path,
                                storage_connection_type_t const type) {
  retcode_t ret = RC_OK;
  UNUSED(test_db_path);

  if (type == STORAGE_CONNECTION_TANGLE) {
    if (mysql_query(&((mariadb_tangle_connection_t*)(connection->actual))->db,
                    "TRUNCATE TABLE " TRANSACTION_TABLE_NAME) != 0 ||
        mysql_query(&((mariadb_tangle_connection_t*)(connection->actual))->db,
                    "TRUNCATE TABLE " MILESTONE_TABLE_NAME) != 0) {
      ret = RC_STORAGE_FAILED_EXECUTE;
    }
  } else if (type == STORAGE_CONNECTION_SPENT_ADDRESSES) {
    if (mysql_query(&((mariadb_spent_addresses_connection_t*)(connection->actual))->db,
                    "TRUNCATE TABLE " SPENT_ADDRESS_TABLE_NAME) != 0) {
      ret = RC_STORAGE_FAILED_EXECUTE;
    }
  }

  if (ret != RC_OK) {
    return ret;
  }

  return storage_connection_destroy(connection);
}
