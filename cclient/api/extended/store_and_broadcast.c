/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/extended/store_and_broadcast.h"
#include "cclient/api/extended/logger.h"

retcode_t iota_client_store_and_broadcast(iota_client_service_t const* const serv,
                                          store_transactions_req_t const* const trytes) {
  retcode_t ret_code = RC_ERROR;
  ret_code = iota_client_store_transactions(serv, trytes);
  if (ret_code == RC_OK) {
    ret_code = iota_client_broadcast_transactions(serv, (broadcast_transactions_req_t*)trytes);
    if (ret_code) {
      log_error(client_extended_logger_id, "sending broadcast transaction failed: %s\n", error_2_string(ret_code));
    }
  } else {
    log_error(client_extended_logger_id, "sending store transaction failed: %s\n", error_2_string(ret_code));
  }
  return ret_code;
}
