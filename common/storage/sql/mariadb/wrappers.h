/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_SQL_MARIADB_WRAPPERS_H__
#define __COMMON_STORAGE_SQL_MARIADB_WRAPPERS_H__

#include <mysql.h>

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t prepare_statement(MYSQL* const db, MYSQL_STMT** const mariadb_statement, char const* const statement);
retcode_t finalize_statement(MYSQL_STMT* const mariadb_statement);

void column_compress_bind(MYSQL_BIND* const bind, size_t const index, void const* const data,
                          enum enum_field_types const type, size_t const num_bytes);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_SQL_MARIADB_WRAPPERS_H__
