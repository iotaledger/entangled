/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/api/core/check_consistency.h"
#include "cclient/api/core/get_trytes.h"

#include "cclient/api/extended/is_promotable.h"
#include "cclient/api/extended/logger.h"

#include "utils/time.h"

retcode_t iota_client_is_promotable(iota_client_service_t const* const serv, flex_trit_t const* const tail_tx,
                                    bool* out_promotable) {
  retcode_t ret_code = RC_ERROR;
  check_consistency_req_t* consistency_req = NULL;
  check_consistency_res_t* consistency_res = NULL;
  get_trytes_res_t* out_trytes = NULL;
  iota_transaction_t tx;
  size_t tx_deserialize_offset = 0;
  uint64_t time_now = 0;
  // Check if transaction wasn't issued before last 6 milestones
  // Milestones are being issued every ~2mins, and 1 minute delay for
  // propagating the network of nodes.
  uint64_t max_depth = (6 * 2 - 1) * (60 * 1000);
  *out_promotable = false;

  consistency_req = check_consistency_req_new();
  consistency_res = check_consistency_res_new();
  if (!consistency_req || !consistency_res) {
    log_error(client_extended_logger_id, "%s: create check consistency request or response failed: %s.\n", __func__,
              error_2_string(ret_code));
    goto done;
  }

  ret_code = hash243_queue_push(&consistency_req->tails, tail_tx);
  if (ret_code) {
    log_error(client_extended_logger_id, "%s: hash queue push failed: %s.\n", __func__, error_2_string(ret_code));
    goto done;
  }

  ret_code = iota_client_check_consistency(serv, consistency_req, consistency_res);
  if (ret_code) {
    log_warning(client_extended_logger_id, "check consistency failed\n");
    goto done;
  }
  if (consistency_res->state) {
    check_consistency_req_free(&consistency_req);
    check_consistency_res_free(&consistency_res);
    out_trytes = get_trytes_res_new();
    if (!out_trytes) {
      log_error(client_extended_logger_id, "%s: create get trytes response failed: %s.\n", __func__,
                error_2_string(ret_code));
      goto done;
    }

    // get attachmentTimestamp and check if above max depth
    ret_code = iota_client_get_trytes(serv, (get_trytes_req_t*)consistency_req, out_trytes);
    if (ret_code == RC_OK) {
      tx_deserialize_offset = transaction_deserialize_from_trits(&tx, out_trytes->trytes->hash, false);
      if (tx_deserialize_offset) {
        // is above max depth
        time_now = current_timestamp_ms();
        log_debug(client_extended_logger_id, "checking depth, attachment_timestamp %ld, time: %ld\n",
                  tx.attachment.attachment_timestamp, time_now);
        if ((tx.attachment.attachment_timestamp < time_now) &&
            ((time_now - tx.attachment.attachment_timestamp) < max_depth)) {
          *out_promotable = true;
        }
      } else {
        ret_code = RC_CCLIENT_TX_DESERIALIZE_FAILED;
        log_error(client_extended_logger_id, "%s: %s.\n", __func__, error_2_string(ret_code));
        goto done;
      }
    }
  }

done:
  check_consistency_req_free(&consistency_req);
  check_consistency_res_free(&consistency_res);
  get_trytes_res_free(&out_trytes);
  return ret_code;
}
