/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/core/find_transactions.h"

#include "cclient/api/extended/find_transaction_objects.h"
#include "cclient/api/extended/get_transaction_objects.h"
#include "cclient/api/extended/logger.h"

retcode_t iota_client_find_transaction_objects(iota_client_service_t const* const serv,
                                               find_transactions_req_t const* const tx_queries,
                                               transaction_array_t* out_tx_objs) {
  retcode_t ret_code = RC_ERROR;
  find_transactions_res_t* find_tx_res = find_transactions_res_new();

  log_debug(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (!find_tx_res) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s create request object failed: %s\n", __func__, error_2_string(ret_code));
    goto done;
  }

  ret_code = iota_client_find_transactions(serv, tx_queries, find_tx_res);
  if (ret_code == RC_OK) {
    ret_code = iota_client_get_transaction_objects(serv, (get_trytes_req_t*)find_tx_res, out_tx_objs);
  }

done:
  find_transactions_res_free(&find_tx_res);
  return ret_code;
}
