/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TANGLE_TANGLE_H__
#define __CONSENSUS_TANGLE_TANGLE_H__

#include <stdbool.h>
#include <stdint.h>

#include "common/errors.h"
#include "common/model/transaction.h"
#include "common/storage/connection.h"
#include "common/storage/defs.h"
#include "common/storage/storage.h"
#include "common/trinary/trit_array.h"
#include "consensus/snapshot/state_delta.h"
#include "utils/containers/hash/hash243_set.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tangle_s {
  connection_t conn;
} tangle_t;

retcode_t iota_tangle_init(tangle_t *const tangle,
                           connection_config_t const *const config);

retcode_t iota_tangle_destroy(tangle_t *const tangle);

/*
 * Transaction operations
 */

retcode_t iota_tangle_transaction_store(tangle_t const *const tangle,
                                        iota_transaction_t const tx);

retcode_t iota_tangle_transaction_load(tangle_t const *const tangle,
                                       transaction_field_t const field,
                                       trit_array_t const *const key,
                                       iota_stor_pack_t *const tx);

retcode_t iota_tangle_transaction_load_hashes(tangle_t const *const tangle,
                                              transaction_field_t const field,
                                              trit_array_t const *const key,
                                              iota_stor_pack_t *const pack);

retcode_t iota_tangle_transaction_load_hashes_of_approvers(
    tangle_t const *const tangle, flex_trit_t const *const approvee_hash,
    iota_stor_pack_t *const pack);

retcode_t iota_tangle_transaction_update_snapshot_index(
    tangle_t const *const tangle, flex_trit_t const *const hash,
    uint64_t const snapshot_index);

retcode_t iota_tangle_transactions_update_snapshot_index(
    tangle_t const *const tangle, hash243_set_t const hashes,
    uint64_t const snapshot_index);

retcode_t iota_tangle_transaction_exist(tangle_t const *const tangle,
                                        transaction_field_t const field,
                                        trit_array_t const *const key,
                                        bool *const exist);

retcode_t iota_tangle_transaction_update_solid_state(
    tangle_t const *const tangle, flex_trit_t const *const hash,
    bool const state);

retcode_t iota_tangle_transactions_update_solid_state(
    tangle_t const *const tangle, hash243_set_t const hashes,
    bool const is_solid);

/*
 * Milestone operations
 */

retcode_t iota_tangle_milestone_store(tangle_t const *const tangle,
                                      iota_milestone_t const *const data_in);

retcode_t iota_tangle_milestone_load(tangle_t const *const tangle,
                                     flex_trit_t const *const hash,
                                     iota_stor_pack_t *const pack);

retcode_t iota_tangle_milestone_load_first(tangle_t const *const tangle,
                                           iota_stor_pack_t *const pack);

retcode_t iota_tangle_milestone_load_last(tangle_t const *const tangle,
                                          iota_stor_pack_t *const pack);

retcode_t iota_tangle_milestone_load_next(tangle_t const *const tangle,
                                          uint64_t const index,
                                          iota_stor_pack_t *const pack);

retcode_t iota_tangle_milestone_exist(tangle_t const *const tangle,
                                      flex_trit_t const *const hash,
                                      bool *const exist);

/*
 * State delta operations
 */

retcode_t iota_tangle_state_delta_store(tangle_t const *const tangle,
                                        uint64_t const index,
                                        state_delta_t const *const delta);

retcode_t iota_tangle_state_delta_load(tangle_t const *const tangle,
                                       uint64_t const index,
                                       state_delta_t *const delta);

/*
 * Utilities
 */

retcode_t iota_tangle_find_tail(tangle_t const *const tangle,
                                trit_array_t const *const tx_hash,
                                trit_array_t *const tail,
                                bool *const found_tail);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TANGLE_TANGLE_H__
