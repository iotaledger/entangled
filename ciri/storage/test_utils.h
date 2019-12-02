/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_STORAGE_TEST_UTILS_H__
#define __CIRI_STORAGE_TEST_UTILS_H__

#include "ciri/storage/connection.h"
#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t storage_test_setup(storage_connection_t* const connection, storage_connection_config_t const* const config,
                             char const* const test_db_path, storage_connection_type_t const type);
retcode_t storage_test_teardown(storage_connection_t* const connection, char const* const test_db_path,
                                storage_connection_type_t const type);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_STORAGE_TEST_UTILS_H__
