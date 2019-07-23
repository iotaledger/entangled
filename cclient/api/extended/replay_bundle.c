/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/extended/replay_bundle.h"
#include "cclient/api/extended/get_bundle.h"
#include "cclient/api/extended/logger.h"
#include "cclient/api/extended/send_trytes.h"

retcode_t iota_client_replay_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                    uint32_t const depth, uint8_t const mwm, flex_trit_t const* const reference,
                                    bundle_transactions_t* const bundle) {
  retcode_t ret_code = RC_ERROR;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  iota_transaction_t* curr_tx = NULL;
  flex_trit_t flex_tx[FLEX_TRIT_SIZE_8019] = {};
  hash8019_array_p trytes = hash8019_array_new();
  log_debug(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (trytes) {
    // get bundle
    ret_code = iota_client_get_bundle(serv, tail_hash, bundle, &bundle_status);
    if (ret_code == RC_OK) {
      if (bundle_status == BUNDLE_VALID) {
        // bundle to trytes
        BUNDLE_FOREACH(bundle, curr_tx) {
          transaction_serialize_on_flex_trits(curr_tx, flex_tx);
          hash_array_push(trytes, flex_tx);
        }

        // send trytes
        ret_code = iota_client_send_trytes(serv, trytes, depth, mwm, reference, false, (transaction_array_t*)bundle);
        if (ret_code != RC_OK) {
          log_error(client_extended_logger_id, "%s iota_client_send_trytes failed: %s\n", __func__,
                    error_2_string(ret_code));
        }
      } else {
        ret_code = RC_CCLIENT_INVALID_BUNDLE;
        log_error(client_extended_logger_id, "%s: %s error code: %d\n", __func__, error_2_string(ret_code),
                  bundle_status);
      }
    }

    hash_array_free(trytes);
  } else {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s new hash array failed: %s\n", __func__, error_2_string(ret_code));
  }
  return ret_code;
}
