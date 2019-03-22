/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/api/core/attach_to_tangle.h"
#include "cclient/api/core/check_consistency.h"
#include "cclient/api/core/get_transactions_to_approve.h"

#include "cclient/api/extended/logger.h"
#include "cclient/api/extended/promote_transaction.h"
#include "cclient/api/extended/store_and_broadcast.h"

retcode_t iota_client_promote_transaction(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                          int const depth, int const mwm, bundle_transactions_t* const bundle,
                                          bundle_transactions_t* const out_bundle) {
  retcode_t ret_code = RC_OK;
  iota_transaction_t spam_transaction;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  get_transactions_to_approve_req_t* gtta_req = NULL;
  get_transactions_to_approve_res_t* gtta_res = NULL;
  attach_to_tangle_req_t* att_req = NULL;
  attach_to_tangle_res_t* att_res = NULL;
  iota_transaction_t* curr_tx = NULL;
  check_consistency_req_t* consistency_req = NULL;
  check_consistency_res_t* consistency_res = NULL;
  flex_trit_t flex_tx[FLEX_TRIT_SIZE_8019] = {};
  log_info(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);

  ret_code = bundle_validator(bundle, &bundle_status);
  if (ret_code != RC_OK) {
    log_error(client_extended_logger_id, "%s bundle_validator error code: %d", __func__, bundle_status);
    goto done;
  }

  // check consistency
  consistency_req = check_consistency_req_new();
  consistency_res = check_consistency_res_new();
  if (!consistency_req || !consistency_res) {
    ret_code = RC_CCLIENT_OOM;
    log_error(client_extended_logger_id, "creating check_consistency failed: %s\n", error_2_string(ret_code));
    goto done;
  }

  if ((ret_code = hash243_queue_push(&consistency_req->tails, tail_hash)) != RC_OK) {
    log_error(client_extended_logger_id, "hash queue push failed: %s\n", error_2_string(ret_code));
    goto done;
  }

  ret_code = iota_client_check_consistency(serv, consistency_req, consistency_res);
  if (ret_code != RC_OK) {
    log_error(client_extended_logger_id, "%s checking consistency failed: %s\n", __func__, error_2_string(ret_code));
    goto done;
  }

  if (consistency_res->state) {
    // consistency req/res are not needed.
    check_consistency_req_free(&consistency_req);
    check_consistency_res_free(&consistency_res);
    // adding spam transaction
    transaction_reset(&spam_transaction);
    bundle_transactions_add(bundle, &spam_transaction);
    bundle_reset_indexes(bundle);

    // get transaction to approve
    gtta_req = get_transactions_to_approve_req_new();
    gtta_res = get_transactions_to_approve_res_new();
    if (!gtta_req || !gtta_res) {
      ret_code = RC_CCLIENT_OOM;
      log_error(client_extended_logger_id, "creating get_transactions_to_approve failed: %s\n",
                error_2_string(ret_code));
      goto done;
    }
    get_transactions_to_approve_req_set_depth(gtta_req, depth);
    get_transactions_to_approve_req_set_reference(gtta_req, tail_hash);
    ret_code = iota_client_get_transactions_to_approve(serv, gtta_req, gtta_res);
    get_transactions_to_approve_req_free(&gtta_req);
    if (ret_code == RC_OK) {
      // attach to tangle
      att_req = attach_to_tangle_req_new();
      att_res = attach_to_tangle_res_new();
      if (!att_req || !att_res) {
        ret_code = RC_CCLIENT_OOM;
        log_error(client_extended_logger_id, "creating get_transactions_to_approve failed: %s\n",
                  error_2_string(ret_code));
        goto done;
      }
      attach_to_tangle_req_init(att_req, gtta_res->trunk, gtta_res->branch, mwm);
      get_transactions_to_approve_res_free(&gtta_res);
      BUNDLE_FOREACH(bundle, curr_tx) {
        if (!transaction_serialize_on_flex_trits(curr_tx, flex_tx)) {
          if ((ret_code = attach_to_tangle_req_trytes_add(att_req, flex_tx)) != RC_OK) {
            log_error(client_extended_logger_id, "%s\n", error_2_string(ret_code));
            goto done;
          }
        } else {
          ret_code = RC_CCLIENT_TX_DESERIALIZE_FAILED;
          log_error(client_extended_logger_id, "%s\n", error_2_string(ret_code));
          goto done;
        }
      }
      ret_code = iota_client_attach_to_tangle(serv, att_req, att_res);
      attach_to_tangle_req_free(&att_req);
      if (ret_code != RC_OK) {
        log_error(client_extended_logger_id, "attach_to_tangle failed: %s\n", error_2_string(ret_code));
        goto done;
      }

      // store and boardcast
      ret_code = iota_client_store_and_broadcast(serv, (store_transactions_req_t*)att_res);
      if (ret_code != RC_OK) {
        log_error(client_extended_logger_id, "%s store_and_broadcast failed: %s\n", __func__, error_2_string(ret_code));
      }
    }

  } else {
    log_warning(client_extended_logger_id, "%s the tail is not consistent: %s\n", __func__,
                consistency_res->info->data);
  }

done:
  check_consistency_req_free(&consistency_req);
  check_consistency_res_free(&consistency_res);
  get_transactions_to_approve_req_free(&gtta_req);
  get_transactions_to_approve_res_free(&gtta_res);
  attach_to_tangle_req_free(&att_req);
  attach_to_tangle_res_free(&att_res);
  return ret_code;
}
