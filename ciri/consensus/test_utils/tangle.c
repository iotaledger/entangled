/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/consensus/tangle/tangle.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "common/storage/test_utils.h"
#include "utils/files.h"

retcode_t tangle_setup(tangle_t *const tangle, storage_connection_config_t *const config, char *test_db_path) {
  return storage_test_setup(&tangle->connection, config, test_db_path, STORAGE_CONNECTION_TANGLE);
}

retcode_t tangle_cleanup(tangle_t *const tangle, char *test_db_path) {
  return storage_test_teardown(&tangle->connection, test_db_path, STORAGE_CONNECTION_TANGLE);
}

void transactions_deserialize(tryte_t const *const *const transactions_trytes, iota_transaction_t **txs,
                              size_t num_transactions, bool compute_hash) {
  flex_trit_t trits[FLEX_TRIT_SIZE_8019];
  for (size_t i = 0; i < num_transactions; ++i) {
    flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION, transactions_trytes[i],
                           NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
    txs[i] = transaction_deserialize(trits, compute_hash);
  }
}

void transactions_free(iota_transaction_t **txs, size_t num_transactions) {
  for (size_t i = 0; i < num_transactions; ++i) {
    transaction_free(txs[i]);
  }
}

retcode_t build_tangle(tangle_t *const tangle, iota_transaction_t **txs, size_t num_transactions) {
  retcode_t ret;
  for (size_t i = 0; i < num_transactions; ++i) {
    if ((ret = iota_tangle_transaction_store(tangle, txs[i])) != RC_OK) {
      return ret;
    }
  }

  return RC_OK;
}
