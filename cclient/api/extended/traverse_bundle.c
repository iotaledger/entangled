/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/get_trytes.h"

#include "cclient/api/extended/logger.h"
#include "cclient/api/extended/traverse_bundle.h"

retcode_t traverse_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                          bundle_transactions_t* const bundle, hash8019_array_p trytes) {
  retcode_t ret_code = RC_ERROR;
  get_trytes_req_t* get_trytes_req = NULL;
  get_trytes_res_t* get_trytes_res = NULL;
  iota_transaction_t tx = {};
  flex_trit_t* tmp_trytes = NULL;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  int64_t current_index = 0, last_index = 0, next_index = 0;
  bool is_tail = true;

  log_debug(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
  get_trytes_req = get_trytes_req_new();
  if (!get_trytes_req) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s get_trytes_req_new failed: %s\n", __func__, error_2_string(ret_code));
    goto cleanup;
  }
  get_trytes_res = get_trytes_res_new();
  if (!get_trytes_res) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s get_trytes_res_new failed: %s\n", __func__, error_2_string(ret_code));
    goto cleanup;
  }

  do {
    ret_code = get_trytes_req_hash_add(get_trytes_req, is_tail ? tail_hash : transaction_trunk(&tx));
    if (ret_code != RC_OK) {
      log_error(client_extended_logger_id, "%s hash243_queue_push failed: %s\n", __func__, error_2_string(ret_code));
      goto cleanup;
    }
    // get trytes from the given hash
    ret_code = iota_client_get_trytes(serv, get_trytes_req, get_trytes_res);
    if (ret_code != RC_OK) {
      log_error(client_extended_logger_id, "%s iota_client_get_trytes failed: %s\n", __func__,
                error_2_string(ret_code));
      goto cleanup;
    }
    // Get the transaction trytes
    tmp_trytes = hash8019_queue_peek(get_trytes_res->trytes);
    if (!tmp_trytes) {
      ret_code = RC_CCLIENT_RES_ERROR;
      log_error(client_extended_logger_id, "%s read transaction trytes failed: %s\n", __func__,
                error_2_string(ret_code));
      goto cleanup;
    }
    // Create a transaction with the received trytes
    transaction_deserialize_from_trits(&tx, tmp_trytes, false);
    transaction_set_hash(&tx, is_tail ? tail_hash : get_trytes_req_hash_get(get_trytes_req, 0));
    if (is_tail) {
      current_index = transaction_current_index(&tx);
      // Check that the first transaction we get is really a tail transaction
      // Its index must be 0 and the list of transactions empty.
      // It is an error if the first transaction has index > 0
      if (current_index != 0) {
        ret_code = RC_CCLIENT_INVALID_TAIL_HASH;
        log_error(client_extended_logger_id, "%s current index is not zero: %s\n", __func__, error_2_string(ret_code));
        goto cleanup;
      }
      last_index = transaction_last_index(&tx);
      memcpy(bundle_hash, transaction_bundle(&tx), FLEX_TRIT_SIZE_243);
      is_tail = false;
      next_index = current_index + 1;
    } else {
      current_index = transaction_current_index(&tx);
      // checking index order
      if (current_index == next_index) {
        next_index++;
      } else {
        // unexpected current index
        ret_code = RC_CCLIENT_INVALID_BUNDLE;
        log_error(client_extended_logger_id, "%s unexpected current_index\n", __func__);
        goto cleanup;
      }
      // checking consistency of bundle hash
      if (memcmp(bundle_hash, transaction_bundle(&tx), FLEX_TRIT_SIZE_243)) {
        ret_code = RC_CCLIENT_INVALID_BUNDLE;
        log_error(client_extended_logger_id, "%s inconsistent bundle hash\n", __func__);
        goto cleanup;
      }
    }

    if (trytes) {
      // save raw trytes if we want them.
      hash_array_push(trytes, tmp_trytes);
    }
    if (bundle) {
      // add this transaction to bundle
      bundle_transactions_add(bundle, &tx);
    }
    // The response is not needed anymore
    hash8019_queue_free(&get_trytes_res->trytes);
    free(hash243_queue_pop(&get_trytes_req->hashes));
  } while (current_index != last_index);
  get_trytes_req_free(&get_trytes_req);
  get_trytes_res_free(&get_trytes_res);
  return ret_code;

cleanup:
  if (bundle) {
    utarray_clear(bundle);
  }
  if (trytes) {
    hash_array_free(trytes);
  }
  get_trytes_req_free(&get_trytes_req);
  get_trytes_res_free(&get_trytes_res);
  return ret_code;
}

retcode_t iota_client_traverse_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                      bundle_transactions_t* const bundle) {
  log_info(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);

  return traverse_bundle(serv, tail_hash, bundle, NULL);
}
