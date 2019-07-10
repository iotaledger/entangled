/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/attach_to_tangle.h"
#include "cclient/api/core/logger.h"
#include "common/helpers/pow.h"
#include "common/model/bundle.h"
#include "utils/containers/hash/hash_array.h"

retcode_t iota_client_attach_to_tangle(iota_client_service_t const* const service,
                                       attach_to_tangle_req_t const* const req, attach_to_tangle_res_t* res) {
  retcode_t result = RC_ERROR;
  char_buffer_t* res_buff = NULL;
  char_buffer_t* req_buff = NULL;
  bundle_transactions_t* bundle = NULL;

  if (!req->trytes) {
    log_error(client_core_logger_id, "[%s:%d] Empty trytes list.\n", __func__, __LINE__);
    return RC_NULL_PARAM;
  }

  if (flex_trits_are_null(req->branch, NUM_FLEX_TRITS_HASH) || flex_trits_are_null(req->trunk, NUM_FLEX_TRITS_HASH)) {
    log_error(client_core_logger_id, "[%s:%d] Empty branch or trunk hash.\n", __func__, __LINE__);
    return RC_NULL_PARAM;
  }

  if (service == NULL) {
    log_info(client_core_logger_id, "[%s:%d] local PoW\n", __func__, __LINE__);
    iota_transaction_t tx = {};
    flex_trit_t* array_elt = NULL;
    flex_trit_t* tx_trytes = NULL;
    // create bundle
    bundle_transactions_new(&bundle);
    HASH_ARRAY_FOREACH(req->trytes, array_elt) {
      transaction_deserialize_from_trits(&tx, array_elt, false);
      bundle_transactions_add(bundle, &tx);
    }

    // PoW on bundle
    if ((result = iota_pow_bundle(bundle, req->trunk, req->branch, req->mwm)) != RC_OK) {
      log_info(client_core_logger_id, "[%s:%d] PoW failed.\n", __func__, __LINE__);
      result = RC_CCLIENT_POW_FAILED;
      goto done;
    }

    // bundle to trytes
    iota_transaction_t* curr_tx = NULL;
    BUNDLE_FOREACH(bundle, curr_tx) {
      tx_trytes = transaction_serialize(curr_tx);
      if (tx_trytes) {
        hash_array_push(res->trytes, tx_trytes);
        free(tx_trytes);
        tx_trytes = NULL;
      } else {
        log_info(client_core_logger_id, "[%s:%d] transaction serialize failed.\n", __func__, __LINE__);
        result = RC_CCLIENT_TX_DESERIALIZE_FAILED;
        goto done;
      }
    }
  } else {
    log_info(client_core_logger_id, "[%s:%d] remote PoW\n", __func__, __LINE__);
    res_buff = char_buffer_new();
    req_buff = char_buffer_new();
    if (req_buff == NULL || res_buff == NULL) {
      log_critical(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_OOM);
      result = RC_OOM;
      goto done;
    }

    result = service->serializer.vtable.attach_to_tangle_serialize_request(req, req_buff);
    if (result != RC_OK) {
      goto done;
    }

    result = iota_service_query(service, req_buff, res_buff);
    if (result != RC_OK) {
      log_error(client_core_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(result));
      goto done;
    }

    result = service->serializer.vtable.attach_to_tangle_deserialize_response(res_buff->data, res);
  }

done:
  char_buffer_free(req_buff);
  char_buffer_free(res_buff);
  bundle_transactions_free(&bundle);
  return result;
}
