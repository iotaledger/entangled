/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/extended/send_transfer.h"
#include "cclient/api/extended/logger.h"
#include "cclient/api/extended/prepare_transfers.h"
#include "cclient/api/extended/send_trytes.h"

retcode_t iota_client_send_transfer(iota_client_service_t const* const serv, int const depth, int const mwm,
                                    bool local_pow, transfer_t** const transfers, uint32_t num_transfer,
                                    flex_trit_t const* const reference, bundle_transactions_t* out_tx_objs) {
  retcode_t ret_code = RC_OK;
  hash8019_array_p raw_tx = hash8019_array_new();
  iota_transaction_t* tx = NULL;
  if (raw_tx) {
    // prepare transfer
    ret_code = iota_client_prepare_transfers(serv, transfers, num_transfer, out_tx_objs);
    if (ret_code == RC_OK) {
      BUNDLE_FOREACH(out_tx_objs, tx) {
        // tx trytes must be in order, from last to 0.
        utarray_insert(raw_tx, transaction_serialize(tx), 0);
      }

      ret_code = iota_client_send_trytes(serv, raw_tx, depth, mwm, reference, local_pow, out_tx_objs);
    }

    hash_array_free(raw_tx);
  } else {
    ret_code = RC_CCLIENT_OOM;
    log_error(client_extended_logger_id, "%s new hash array failed: %s\n", __func__, error_2_string(ret_code));
  }
  return ret_code;
}
