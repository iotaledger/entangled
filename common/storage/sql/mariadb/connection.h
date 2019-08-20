/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_MARIADB_CONNECTION_H__
#define __COMMON_STORAGE_SQL_MARIADB_CONNECTION_H__

#include <mysql.h>

#include "common/storage/connection.h"
#include "common/storage/sql/statements.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mariadb_tangle_connection_s {
  MYSQL db;
  tangle_statements_t statements;
} mariadb_tangle_connection_t;

typedef struct mariadb_spent_addresses_connection_s {
  MYSQL db;
  spent_addresses_statements_t statements;
} mariadb_spent_addresses_connection_t;

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_SQL_MARIADB_CONNECTION_H__
