/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/api/core/attach_to_tangle.h"
#include "cclient/api/core/get_transactions_to_approve.h"

#include "cclient/api/extended/logger.h"
#include "cclient/api/extended/send_trytes.h"
#include "cclient/api/extended/store_and_broadcast.h"

retcode_t iota_client_send_trytes(iota_client_service_t const* const serv, hash8019_array_p const trytes,
                                  uint32_t const depth, uint8_t const mwm, flex_trit_t const* const reference,
                                  bool const local_pow, transaction_array_t* out_transactions) {
  retcode_t ret_code = RC_ERROR;
  get_transactions_to_approve_req_t* tx_approve_req = get_transactions_to_approve_req_new();
  get_transactions_to_approve_res_t* tx_approve_res = get_transactions_to_approve_res_new();
  attach_to_tangle_req_t* attach_req = NULL;
  attach_to_tangle_res_t* attach_res = NULL;
  flex_trit_t* elt = NULL;
  size_t tx_deserialize_offset = 0;
  iota_transaction_t tx;

  if (!tx_approve_req || !tx_approve_res) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "creating get transaction to approve request or response failed: %s\n",
              error_2_string(ret_code));
    goto done;
  }

  if (reference) {
    if ((ret_code = get_transactions_to_approve_req_set_reference(tx_approve_req, reference)) != RC_OK) {
      log_error(client_extended_logger_id, "set reference failed: %s\n", error_2_string(ret_code));
      goto done;
    }
  }
  tx_approve_req->depth = depth;
  // get tx to approve
  ret_code = iota_client_get_transactions_to_approve(serv, tx_approve_req, tx_approve_res);
  if (ret_code) {
    log_error(client_extended_logger_id, "sending get transaction to approve failed: %s\n", error_2_string(ret_code));
    goto done;
  }

  attach_req = attach_to_tangle_req_new();
  attach_res = attach_to_tangle_res_new();
  if (!attach_req || !attach_res) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "creating attach to tangle request or response failed: %s\n",
              error_2_string(ret_code));
    goto done;
  }
  memcpy(attach_req->branch, tx_approve_res->branch, FLEX_TRIT_SIZE_243);
  memcpy(attach_req->trunk, tx_approve_res->trunk, FLEX_TRIT_SIZE_243);
  attach_req->mwm = mwm;
  // reuse hash_array instead of copy.
  hash_array_free(attach_req->trytes);
  attach_req->trytes = trytes;

  get_transactions_to_approve_req_free(&tx_approve_req);
  get_transactions_to_approve_res_free(&tx_approve_res);

  // attach to tangle
  ret_code = iota_client_attach_to_tangle(local_pow ? NULL : serv, attach_req, attach_res);

  attach_req->trytes = NULL;
  attach_to_tangle_req_free(&attach_req);

  if (ret_code) {
    log_error(client_extended_logger_id, "sending attach to tangle failed: %s\n", error_2_string(ret_code));
    goto done;
  }

  // store and broadcast
  ret_code = iota_client_store_and_broadcast(serv, (store_transactions_req_t*)attach_res);
  if (ret_code) {
    log_error(client_extended_logger_id, "sending store and broadcast failed: %s\n", error_2_string(ret_code));
    goto done;
  }

  // trytes to transaction objects
  HASH_ARRAY_FOREACH(attach_res->trytes, elt) {
    tx_deserialize_offset = transaction_deserialize_from_trits(&tx, elt, true);
    if (tx_deserialize_offset) {
      transaction_array_push_back(out_transactions, &tx);
    } else {
      ret_code = RC_CCLIENT_TX_DESERIALIZE_FAILED;
      log_error(client_extended_logger_id, "%s: %s.\n", __func__, error_2_string(ret_code));
      goto done;
    }
  }

done:
  get_transactions_to_approve_req_free(&tx_approve_req);
  get_transactions_to_approve_res_free(&tx_approve_res);
  attach_to_tangle_res_free(&attach_res);
  attach_to_tangle_req_free(&attach_req);
  return ret_code;
}
