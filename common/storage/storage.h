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
#include "consensus/snapshot/state_delta.h"

// Forward declaration
typedef struct iota_milestone_s iota_milestone_t;
typedef struct _iota_transaction* iota_transaction_t;

#ifdef __cplusplus
extern "C" {
#endif

extern retcode_t iota_stor_init(connection_t const* const conn,
                                connection_config_t const* const config);

extern retcode_t iota_stor_destroy(connection_t const* const conn);

/*
 * Transaction operations
 */

typedef enum transaction_field_e {
  TRANSACTION_FIELD_NONE,
  TRANSACTION_FIELD_HASH,
  TRANSACTION_FIELD_ADDRESS
} transaction_field_t;

extern retcode_t iota_stor_transaction_store(connection_t const* const conn,
                                             iota_transaction_t const data_in);

extern retcode_t iota_stor_transaction_load(connection_t const* const conn,
                                            transaction_field_t const field,
                                            trit_array_t const* const key,
                                            iota_stor_pack_t* const pack);

extern retcode_t iota_stor_transaction_exist(connection_t const* const conn,
                                             transaction_field_t const field,
                                             trit_array_t const* const key,
                                             bool* const exist);

extern retcode_t iota_stor_transaction_update_snapshot_index(
    connection_t const* const conn, flex_trit_t const* const hash,
    uint64_t const snapshot_index);

extern retcode_t iota_stor_transactions_update_snapshot_index(
    connection_t const* const conn, hash243_set_t const hashes,
    uint64_t const snapshot_index);

extern retcode_t iota_stor_transaction_update_solid_state(
    connection_t const* const conn, flex_trit_t const* const hash,
    bool const is_solid);

extern retcode_t iota_stor_transactions_update_solid_state(
    connection_t const* const conn, hash243_set_t const hashes,
    bool const is_solid);

extern retcode_t iota_stor_transaction_load_hashes(
    connection_t const* const conn, transaction_field_t const field,
    trit_array_t const* const key, iota_stor_pack_t* const pack);

extern retcode_t iota_stor_transaction_load_hashes_of_approvers(
    connection_t const* const conn, flex_trit_t const* const approvee_hash,
    iota_stor_pack_t* const pack);

/*
 * Milestone operations
 */

extern retcode_t iota_stor_milestone_store(
    connection_t const* const conn, iota_milestone_t const* const data_in);

extern retcode_t iota_stor_milestone_load(connection_t const* const conn,
                                          flex_trit_t const* const hash,
                                          iota_stor_pack_t* const pack);

extern retcode_t iota_stor_milestone_load_first(connection_t const* const conn,
                                                iota_stor_pack_t* const pack);

extern retcode_t iota_stor_milestone_load_last(connection_t const* const conn,
                                               iota_stor_pack_t* const pack);

extern retcode_t iota_stor_milestone_load_next(connection_t const* const conn,
                                               uint64_t const index,
                                               iota_stor_pack_t* const pack);

extern retcode_t iota_stor_milestone_exist(connection_t const* const conn,
                                           flex_trit_t const* const hash,
                                           bool* const exist);

/*
 * State delta operations
 */

extern retcode_t iota_stor_state_delta_store(connection_t const* const conn,
                                             uint64_t const index,
                                             state_delta_t const* const delta);

extern retcode_t iota_stor_state_delta_load(connection_t const* const conn,
                                            uint64_t const index,
                                            state_delta_t* const delta);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_STORAGE_H__
