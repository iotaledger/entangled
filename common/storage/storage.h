/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_STORAGE_H__
#define __COMMON_STORAGE_STORAGE_H__

#include <stdbool.h>

#include "common/errors.h"
#include "common/storage/connection.h"
#include "common/storage/defs.h"
#include "common/storage/pack.h"
#include "common/trinary/trit_array.h"

// Forward declaration
typedef struct iota_milestone_s iota_milestone_t;
typedef struct _iota_transaction* iota_transaction_t;

#ifdef __cplusplus
extern "C" {
#endif

extern retcode_t iota_stor_init(const connection_t* const conn,
                                const connection_config_t* const config);

extern retcode_t iota_stor_destroy(const connection_t* const conn);

/*
 * Transaction operations
 */

extern retcode_t iota_stor_transaction_store(const connection_t* const conn,
                                             const iota_transaction_t data_in);

extern retcode_t iota_stor_transaction_load(const connection_t* const conn,
                                            const char* col_name,
                                            const trit_array_p key,
                                            iota_stor_pack_t* pack);

extern retcode_t iota_stor_transaction_exist(const connection_t* const conn,
                                             const char* index_name,
                                             const trit_array_p key,
                                             bool* exist);

extern retcode_t iota_stor_transaction_update(const connection_t* const conn,
                                              const char* index_name,
                                              const trit_array_p key,
                                              const iota_transaction_t data_in);

extern retcode_t iota_stor_transaction_load_hashes(
    const connection_t* const conn, const char* col_name,
    const trit_array_p key, iota_stor_pack_t* pack);

extern retcode_t iota_stor_transaction_load_hashes_of_approvers(
    const connection_t* const conn, const flex_trit_t* approvee_hash,
    iota_stor_pack_t* pack);

/*
 * Milestone operations
 */

extern retcode_t iota_stor_milestone_store(const connection_t* const conn,
                                           const iota_milestone_t* data_in);

extern retcode_t iota_stor_milestone_load(const connection_t* const conn,
                                          const char* col_name,
                                          const trit_array_p key,
                                          iota_stor_pack_t* pack);

extern retcode_t iota_stor_milestone_load_latest(const connection_t* const conn,
                                                 iota_stor_pack_t* pack);

extern retcode_t iota_stor_milestone_load_next(const connection_t* const conn,
                                               uint64_t index,
                                               iota_stor_pack_t* pack);

extern retcode_t iota_stor_milestone_exist(const connection_t* const conn,
                                           const char* index_name,
                                           const trit_array_p key, bool* exist);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_STORAGE_H__
