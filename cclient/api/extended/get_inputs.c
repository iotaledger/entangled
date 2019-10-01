/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/api/extended/get_inputs.h"
#include "cclient/api/core/find_transactions.h"
#include "cclient/api/core/get_balances.h"
#include "cclient/api/extended/get_new_address.h"
#include "cclient/api/extended/logger.h"
#include "common/helpers/sign.h"

retcode_t iota_client_get_inputs(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                 address_opt_t const addr_opt, int64_t const threshold, inputs_t* const out_inputs) {
  retcode_t ret_code = RC_ERROR;
  get_balances_req_t* balances_req = get_balances_req_new();
  get_balances_res_t* balances_res = get_balances_res_new();

  out_inputs->total_balance = 0;

  flex_trit_t* addr = NULL;
  bool is_used = false;
  uint64_t addr_index, addr_index_end;
  if (addr_opt.total == 0) {
    addr_index = 0;
    addr_index_end = (uint64_t)-1;
  } else {
    addr_index = addr_opt.start;
    addr_index_end = addr_opt.total;
  }

  for (; addr_index < addr_index_end; addr_index++) {
    balances_req->addresses = NULL;

    log_debug(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
    if (!balances_req || !balances_res) {
      ret_code = RC_OOM;
      log_error(client_extended_logger_id, "%s create get balances request or response object failed: %s\n", __func__,
                error_2_string(ret_code));
      goto done;
    }

    addr = iota_sign_address_gen_flex_trits(seed, addr_index, addr_opt.security);
    if (addr) {
      ret_code = hash243_queue_push(&balances_req->addresses, addr);
      if (ret_code) {
        log_error(client_extended_logger_id, "%s:%d hash queue push failed: %s\n", __func__, __LINE__,
                  error_2_string(ret_code));
        goto done;
      }
    } else {
      // gen address failed.
      ret_code = RC_OOM;
      log_error(client_extended_logger_id, "%s address generation failed: %s\n", __func__, error_2_string(ret_code));
      goto done;
    }

    // For request without `addr_opt.total`, end this loop if the current addr is an unused address
    if (addr_opt.total == 0) {
      ret_code = is_used_address(serv, addr, &is_used, false, NULL);
      if (ret_code == RC_OK && !is_used) {
        break;
      }
    }

    balances_req->threshold = 100;  // currently `100` should be used

    ret_code = iota_client_get_balances(serv, balances_req, balances_res);
    if (ret_code == RC_OK) {
      // expect balance value is in order.
      if (out_inputs->total_balance < threshold) {
        input_t tmp_input = {};
        tmp_input.balance = get_balances_res_balances_at(balances_res, 0);
        tmp_input.key_index = addr_index;
        tmp_input.security = addr_opt.security;
        memcpy(tmp_input.address, addr, FLEX_TRIT_SIZE_243);
        if ((ret_code = inputs_append(out_inputs, &tmp_input)) != RC_OK) {
          log_error(client_extended_logger_id, "%s get new address failed: %s\n", __func__, error_2_string(ret_code));
          goto done;
        }
      } else {
        break;
      }
    }

    free(addr);
    addr = NULL;
    hash243_queue_free(&balances_req->addresses);
    utarray_clear(balances_res->balances);
  }

  // check if balance is sufficient
  if (out_inputs->total_balance < threshold) {
    ret_code = RC_CCLIENT_INSUFFICIENT_BALANCE;
    log_warning(client_extended_logger_id, "insufficient balance\n");
  }

done:
  free(addr);
  get_balances_req_free(&balances_req);
  get_balances_res_free(&balances_res);
  return ret_code;
}
