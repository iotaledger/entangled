/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/broadcast_transactions.h"
#include "cclient/api/core/get_trytes.h"

#include "cclient/api/extended/broadcast_bundle.h"
#include "cclient/api/extended/logger.h"
#include "cclient/api/extended/traverse_bundle.h"

retcode_t iota_client_broadcast_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                       bundle_transactions_t* const bundle) {
  retcode_t ret_code = RC_ERROR;
  broadcast_transactions_req_t* broadcast_transactions_req = NULL;

  log_debug(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);

  broadcast_transactions_req = broadcast_transactions_req_new();
  if (!broadcast_transactions_req) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s broadcast_transactions_req_new failed: %s\n", __func__,
              error_2_string(ret_code));
    goto cleanup;
  }

  ret_code = traverse_bundle(serv, tail_hash, bundle, broadcast_transactions_req->trytes);
  if (ret_code != RC_OK) {
    log_error(client_extended_logger_id, "%s traverse_bundle failed: %s\n", __func__, error_2_string(ret_code));
    goto cleanup;
  }
  // Call the broadcast_transactions API
  ret_code = iota_client_broadcast_transactions(serv, broadcast_transactions_req);
  if (ret_code != RC_OK) {
    log_error(client_extended_logger_id, "%s iota_client_broadcast_transactions failed: %s\n", __func__,
              error_2_string(ret_code));
    goto cleanup;
  }

cleanup:
  broadcast_transactions_req_free(&broadcast_transactions_req);
  return ret_code;
}
