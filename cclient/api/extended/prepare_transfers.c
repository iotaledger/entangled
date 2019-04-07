/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/extended/prepare_transfers.h"
#include "cclient/api/extended/logger.h"

retcode_t iota_client_prepare_transfers(iota_client_service_t const* const serv, transfer_t** const transfers,
                                        uint32_t const num_transfer, bundle_transactions_t* out_bundle) {
  retcode_t ret_code = RC_OK;
  iota_transaction_t TX = {};
  iota_transaction_t* tx = NULL;
  transfer_iterator_t* transfer_iterator = NULL;
  Kerl kerl = {};

  transfer_iterator = transfer_iterator_new(transfers, num_transfer, &kerl, &TX);

  if (transfer_iterator) {
    for (tx = transfer_iterator_next(transfer_iterator); tx; tx = transfer_iterator_next(transfer_iterator)) {
      bundle_transactions_add(out_bundle, tx);
    }
  } else {
    ret_code = RC_CCLIENT_INVALID_TRANSFER;
    log_error(client_extended_logger_id, "preparing transfer failed: %s\n", __func__, error_2_string(ret_code));
  }

  return ret_code;
}
