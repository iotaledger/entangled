/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/extended/promote_transaction.h"
#include "cclient/api/core/check_consistency.h"
#include "cclient/api/extended/logger.h"
#include "cclient/api/extended/send_transfer.h"

retcode_t iota_client_promote_transaction(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                          uint8_t security, uint32_t const depth, uint8_t const mwm,
                                          transfer_array_t* spam_transfers, bundle_transactions_t* out_bundle) {
  retcode_t ret_code = RC_ERROR;
  if (out_bundle == NULL) {
    ret_code = RC_NULL_PARAM;
    log_error(client_extended_logger_id, "%s the out_bundle parameter cannot be NULL \n", __func__,
              error_2_string(ret_code));
    goto done;
  }

  // check consistency
  check_consistency_req_t* consistency_req = check_consistency_req_new();
  check_consistency_res_t* consistency_res = check_consistency_res_new();
  if (!consistency_req || !consistency_res) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "creating check_consistency failed: %s\n", error_2_string(ret_code));
    goto done;
  }

  if ((ret_code = hash243_queue_push(&consistency_req->tails, tail_hash)) != RC_OK) {
    log_error(client_extended_logger_id, "hash queue push failed: %s\n", error_2_string(ret_code));
    goto done;
  }

  if ((ret_code = iota_client_check_consistency(serv, consistency_req, consistency_res)) != RC_OK) {
    log_error(client_extended_logger_id, "%s checking consistency failed: %s\n", __func__, error_2_string(ret_code));
    goto done;
  }

  if (consistency_res->state) {
    transfer_t* spam = transfer_array_at(spam_transfers, 0);
    ret_code = iota_client_send_transfer(serv, spam->address, security, depth, mwm, false, spam_transfers, NULL,
                                         tail_hash, NULL, out_bundle);

  } else {
    log_warning(client_extended_logger_id, "%s the tail is not consistent: %s\n", __func__,
                consistency_res->info->data);
    ret_code = RC_CCLIENT_NOT_PROMOTABLE;
  }

done:
  check_consistency_req_free(&consistency_req);
  check_consistency_res_free(&consistency_res);
  return ret_code;
}
