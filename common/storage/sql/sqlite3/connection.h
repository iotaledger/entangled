/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_SQLITE3_CONNECTION_H__
#define __COMMON_STORAGE_SQL_SQLITE3_CONNECTION_H__

#include "common/storage/connection.h"
#include "common/storage/sql/statements.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sqlite3_connection_s {
  sqlite3* db;
  iota_statements_t statements;
} sqlite3_connection_t;

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_SQL_SQLITE3_CONNECTION_H__
