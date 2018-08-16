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

typedef struct iota_transactions_pack {
  iota_transaction_t* txs;
  size_t txs_capacity;
  size_t num_loaded;
  bool insufficient_capacity;
} iota_transactions_pack;

typedef struct iota_hashes_pack {
  trit_array_p* hashes;
  size_t hashes_capacity;
  size_t num_loaded;
  bool insufficient_capacity;
} iota_hashes_pack;

extern retcode_t iota_stor_init();
extern retcode_t iota_stor_destroy();
/*index_name = column name by to compare to key*/
extern retcode_t iota_stor_store(const connection_t* const conn,
                                 const iota_transaction_t data_in);
extern retcode_t iota_stor_load(const connection_t* const conn,
                                const char* col_name, const trit_array_p key,
                                iota_transactions_pack* pack);
extern retcode_t iota_stor_exist(const connection_t* const conn,
                                 const char* index_name, const trit_array_p key,
                                 bool* exist);

extern retcode_t iota_stor_update(const connection_t* const conn,
                                  const char* index_name,
                                  const trit_array_p key,
                                  const iota_transaction_t data_in);

extern retcode_t iota_stor_load_hashes(const connection_t* const conn,
                                       const char* col_name,
                                       const trit_array_p key,
                                       iota_hashes_pack* pack);

extern retcode_t iota_stor_load_hashes_approvers(
    const connection_t* const conn, const trit_array_p approvee_hash,
    iota_hashes_pack* pack);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_STORAGE_H__
