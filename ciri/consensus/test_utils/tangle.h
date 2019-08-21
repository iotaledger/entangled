/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TEST_UTILS_TANGLE_H__
#define __CONSENSUS_TEST_UTILS_TANGLE_H__

#include "ciri/consensus/tangle/tangle.h"
#include "common/errors.h"
#include "common/storage/connection.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t tangle_setup(tangle_t *const tangle, storage_connection_config_t *const config, char *test_db_path);

retcode_t tangle_cleanup(tangle_t *const tangle, char *test_db_path);

void transactions_deserialize(tryte_t const *const *const transactions_trytes, iota_transaction_t **txs,
                              size_t num_transactions, bool compute_hash);

retcode_t build_tangle(tangle_t *const tangle, iota_transaction_t **txs, size_t num_transactions);

void transactions_free(iota_transaction_t **txs, size_t num_transactions);

#ifdef __cplusplus
}
#endif

#endif  //__CONSENSUS_TEST_UTILS_TANGLE_H__
