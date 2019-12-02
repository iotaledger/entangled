/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_STORAGE_SQL_SQLITE3_WRAPPERS_H__
#define __CIRI_STORAGE_SQL_SQLITE3_WRAPPERS_H__

#include <sqlite3.h>

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t prepare_statement(sqlite3* const db, sqlite3_stmt** const sqlite_statement, char const* const statement);
retcode_t finalize_statement(sqlite3_stmt* const sqlite_statement);

retcode_t begin_transaction(sqlite3* const db);
retcode_t end_transaction(sqlite3* const db);
retcode_t rollback_transaction(sqlite3* const db);

retcode_t column_compress_bind(sqlite3_stmt* const statement, size_t const index, flex_trit_t const* const flex_trits,
                               size_t const num_bytes);
void column_decompress_load(sqlite3_stmt* const statement, size_t const index, flex_trit_t* const flex_trits,
                            size_t const num_bytes);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_STORAGE_SQL_SQLITE3_WRAPPERS_H__
