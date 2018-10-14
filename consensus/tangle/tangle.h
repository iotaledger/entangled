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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tangle_s {
  connection_t conn;
} tangle_t;

extern retcode_t iota_tangle_init(tangle_t *tangle,
                                  const connection_config_t *config);
extern retcode_t iota_tangle_destroy(tangle_t *tangle);

/*
 * Transaction operations
 */

extern retcode_t iota_tangle_transaction_store(const tangle_t *const tangle,
                                               const iota_transaction_t tx);

extern retcode_t iota_tangle_transaction_load(const tangle_t *const tangle,
                                              transaction_field_t const field,
                                              const trit_array_p key,
                                              iota_stor_pack_t *tx);

extern retcode_t iota_tangle_transaction_load_hashes(
    const tangle_t *const tangle, transaction_field_t const field,
    const trit_array_p key, iota_stor_pack_t *pack);

extern retcode_t iota_tangle_transaction_load_hashes_of_approvers(
    const tangle_t *const tangle, const flex_trit_t *approvee_hash,
    iota_stor_pack_t *pack);

extern retcode_t iota_tangle_transaction_update_snapshot_index(
    const tangle_t *const tangle, flex_trit_t *const hash,
    uint64_t snapshot_index);

extern retcode_t iota_tangle_transaction_exist(const tangle_t *const tangle,
                                               transaction_field_t const field,
                                               const trit_array_p key,
                                               bool *exist);

/*
 * Milestone operations
 */

extern retcode_t iota_tangle_milestone_store(const tangle_t *const tangle,
                                             const iota_milestone_t *data_in);

extern retcode_t iota_tangle_milestone_load(const tangle_t *const tangle,
                                            const trit_array_p key,
                                            iota_stor_pack_t *pack);

extern retcode_t iota_tangle_milestone_load_first(const tangle_t *const tangle,
                                                  iota_stor_pack_t *pack);

extern retcode_t iota_tangle_milestone_load_last(const tangle_t *const tangle,
                                                 iota_stor_pack_t *pack);

extern retcode_t iota_tangle_milestone_load_next(const tangle_t *const tangle,
                                                 uint64_t index,
                                                 iota_stor_pack_t *pack);

extern retcode_t iota_tangle_milestone_exist(const tangle_t *const tangle,
                                             const trit_array_p key,
                                             bool *exist);

/*
 * Utilities
 */

retcode_t iota_tangle_find_tail(const tangle_t *const tangle,
                                trit_array_t *tx_hash, trit_array_t *tail,
                                bool *found_tail);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TANGLE_TANGLE_H__
