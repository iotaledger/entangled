/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_STORAGE_H__
#define __COMMON_STORAGE_STORAGE_H__

#include <stdbool.h>
#include <stdint.h>
#include "common/errors.h"
#include "common/model/transaction.h"
#include "common/storage/connection.h"
#include "common/trinary/trit_array.h"

#ifdef __cplusplus
extern "C" {
#endif

extern retcode_t iota_stor_init();
extern retcode_t iota_stor_destroy();
/*index_name = column name by to compare to key*/
extern retcode_t iota_stor_store(const connection_t* const conn,
                                 const iota_transaction_t data_in);
extern retcode_t iota_stor_load(const connection_t* const conn,
                                const char* index_name, const trit_array_p key,
                                iota_transaction_t data_out[],
                                size_t max_num_txs, size_t* num_loaded);
extern retcode_t iota_stor_exist(const connection_t* const conn,
                                 const char* index_name, const trit_array_p key,
                                 bool* exist);

extern retcode_t iota_stor_update(const connection_t* const conn,
                                  const char* index_name,
                                  const trit_array_p key,
                                  const iota_transaction_t data_in);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_STORAGE_H__
