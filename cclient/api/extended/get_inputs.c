/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/api/core/get_balances.h"

#include "cclient/api/extended/get_inputs.h"
#include "cclient/api/extended/get_new_address.h"
#include "cclient/api/extended/logger.h"

retcode_t iota_client_get_inputs(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                 address_opt_t const addr_opt, uint64_t const threshold, inputs_t* const out_input) {
  retcode_t ret_code = RC_OK;
  hash243_queue_entry_t* q_iter = NULL;
  size_t counter = 0;
  get_balances_req_t* balances_req = get_balances_req_new();
  get_balances_res_t* balances_res = get_balances_res_new();

  log_info(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (!balances_req || !balances_res) {
    ret_code = RC_CCLIENT_OOM;
    log_error(client_extended_logger_id, "%s create get balances request or response object failed: %s\n", __func__,
              error_2_string(ret_code));
    return ret_code;
  }

  out_input->total_balance = 0;

  // get address list
  ret_code = iota_client_get_new_address(serv, seed, addr_opt, &balances_req->addresses);
  if (ret_code) {
    log_error(client_extended_logger_id, "%s get new address failed: %s\n", __func__, error_2_string(ret_code));
    goto done;
  }

  balances_req->threshold = 100;  // currently `100` should be used
  ret_code = iota_client_get_balances(serv, balances_req, balances_res);
  if (ret_code == RC_OK && balances_req->addresses) {
    // expect balance value is in order.
    CDL_FOREACH(balances_req->addresses, q_iter) {
      if (out_input->total_balance < threshold) {
        out_input->total_balance += get_balances_res_balances_at(balances_res, counter);
        ret_code = hash243_queue_push(&out_input->addresses, q_iter->hash);
        if (ret_code) {
          log_error(client_extended_logger_id, "%s get new address failed: %s\n", __func__, error_2_string(ret_code));
          goto done;
        }
      }
      counter++;
    }
    // check if balance is sufficient
    if (out_input->total_balance < threshold) {
      ret_code = RC_CCLIENT_INSUFFICIENT_BALANCE;
      log_warning(client_extended_logger_id, "insufficient balance\n");
    }
  }

done:
  get_balances_req_free(&balances_req);
  get_balances_res_free(&balances_res);
  return ret_code;
}
