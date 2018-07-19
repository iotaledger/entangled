/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/storage/storage.h"
#include <stdint.h>

#include <sqlite3.h>

retcode_t iota_stor_store(const connection_t* const conn,
                          const trit_array_p key, const trit_array_p data_in) {}
retcode_t iota_stor_load(const connection_t* const conn, const char* index_name,
                         const trit_array_p key, trit_array_p data_out) {}
retcode_t iota_stor_exist(const connection_t* const conn,
                          const char* index_name, const trit_array_p key,
                          bool* exist) {}
retcode_t iota_stor_find(const connection_t* const conn, const char* index_name,
                         const trit_array_p key, bool* found,
                         trit_array_p data_out) {}
retcode_t iota_stor_update(const connection_t* const conn,
                           const char* index_name, const trit_array_p key,
                           const trit_array_p data_in) {}
