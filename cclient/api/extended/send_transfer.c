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

retcode_t iota_client_send_transfer(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                    uint8_t security, uint32_t const depth, uint8_t const mwm, bool local_pow,
                                    transfer_array_t const* const transfers, flex_trit_t const* const remainder_address,
                                    flex_trit_t const* const reference, inputs_t const* const inputs,
                                    bundle_transactions_t* out_tx_objs) {
  retcode_t ret_code = RC_ERROR;
  hash8019_array_p raw_tx = hash8019_array_new();
  iota_transaction_t* tx = NULL;
  flex_trit_t serialized_value[FLEX_TRIT_SIZE_8019];

  if (raw_tx) {
    // prepare transfer
    ret_code =
        iota_client_prepare_transfers(serv, seed, security, transfers, remainder_address, inputs, true, 0, out_tx_objs);
    if (ret_code == RC_OK) {
      BUNDLE_FOREACH(out_tx_objs, tx) {
        // tx trytes must be in order, from last to 0.
        transaction_serialize_on_flex_trits(tx, serialized_value);
        utarray_insert(raw_tx, serialized_value, 0);
      }

      ret_code = iota_client_send_trytes(serv, raw_tx, depth, mwm, reference, local_pow, out_tx_objs);
    }

    hash_array_free(raw_tx);
  } else {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s new hash array failed: %s\n", __func__, error_2_string(ret_code));
  }
  return ret_code;
}
