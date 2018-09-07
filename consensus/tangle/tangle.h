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

extern retcode_t iota_tangle_transaction_store(
    const tangle_t *const tangle, const iota_transaction_t data_in);

extern retcode_t iota_tangle_transaction_load(const tangle_t *const tangle,
                                              const char *col_name,
                                              const trit_array_p key,
                                              iota_stor_pack_t *data_out);

extern retcode_t iota_tangle_transaction_exist(const tangle_t *const tangle,
                                               const char *index_name,
                                               const trit_array_p key,
                                               bool *exist);

extern retcode_t iota_tangle_transaction_update(
    const tangle_t *const tangle, const char *index_name,
    const trit_array_p key, const iota_transaction_t data_in);

extern retcode_t iota_tangle_transaction_load_hashes(
    const tangle_t *const tangle, const char *index_name,
    const trit_array_p key, iota_stor_pack_t *pack);

extern retcode_t iota_tangle_transaction_load_hashes_of_approvers(
    const tangle_t *const tangle, const trit_array_p approvee_hash,
    iota_stor_pack_t *pack);

/*
 * Milestone operations
 */

extern retcode_t iota_tangle_milestone_store(const tangle_t *const tangle,
                                             const iota_milestone_t *data_in);

extern retcode_t iota_tangle_milestone_load(const tangle_t *const tangle,
                                            const char *col_name,
                                            const trit_array_p key,
                                            iota_stor_pack_t *pack);

extern retcode_t iota_tangle_milestone_exist(const tangle_t *const tangle,
                                             const char *index_name,
                                             const trit_array_p key,
                                             bool *exist);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_TANGLE_TANGLE_H__
