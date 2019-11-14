/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_SQLITE3_CONNECTION_H__
#define __COMMON_STORAGE_SQL_SQLITE3_CONNECTION_H__

#include <sqlite3.h>

#include "ciri/storage/connection.h"
#include "ciri/storage/sql/statements.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sqlite3_tangle_connection_s {
  sqlite3* db;
  tangle_statements_t statements;
} sqlite3_tangle_connection_t;

typedef struct sqlite3_spent_addresses_connection_s {
  sqlite3* db;
  spent_addresses_statements_t statements;
} sqlite3_spent_addresses_connection_t;

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_SQL_SQLITE3_CONNECTION_H__
