/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/get_trytes.h"

#include "cclient/api/extended/get_transaction_objects.h"
#include "cclient/api/extended/logger.h"

retcode_t iota_client_get_transaction_objects(iota_client_service_t const* const serv,
                                              get_trytes_req_t* const tx_hashes, transaction_array_t* out_tx_objs) {
  retcode_t ret_code = RC_ERROR;
  hash8019_queue_entry_t* q_iter = NULL;
  iota_transaction_t tx;
  size_t tx_deserialize_offset = 0;
  get_trytes_res_t* out_trytes = get_trytes_res_new();

  if (!out_trytes) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s: create get trytes response failed: %s\n", __func__,
              error_2_string(ret_code));
    goto done;
  }

  ret_code = iota_client_get_trytes(serv, tx_hashes, out_trytes);
  if (ret_code == RC_OK) {
    CDL_FOREACH(out_trytes->trytes, q_iter) {
      tx_deserialize_offset = transaction_deserialize_from_trits(&tx, q_iter->hash, true);
      if (tx_deserialize_offset) {
        transaction_array_push_back(out_tx_objs, &tx);
      } else {
        ret_code = RC_CCLIENT_TX_DESERIALIZE_FAILED;
        log_error(client_extended_logger_id, "%s: %s.\n", __func__, error_2_string(ret_code));
        goto done;
      }
    }
  }

done:
  get_trytes_res_free(&out_trytes);
  return ret_code;
}
