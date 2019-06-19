/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <utarray.h>

#include "cclient/api/core/get_balances.h"
#include "cclient/api/extended/get_account_data.h"
#include "cclient/api/extended/get_new_address.h"
#include "cclient/api/extended/logger.h"
#include "common/helpers/sign.h"

static const UT_icd ut_balance_icd = {sizeof(uint64_t), NULL, NULL, NULL};

void account_data_init(account_data_t* const account) {
  account->balance = 0;
  memset(account->latest_address, 0, FLEX_TRIT_SIZE_243);
  account->addresses = NULL;
  account->transactions = NULL;
  account->balances = NULL;
  utarray_new(account->balances, &ut_balance_icd);
}

void account_data_clear(account_data_t* const account) {
  hash243_queue_free(&account->transactions);
  hash243_queue_free(&account->addresses);
  utarray_free(account->balances);
}

uint64_t account_data_get_balance(account_data_t* const account, size_t index) {
  return *(uint64_t*)utarray_eltptr(account->balances, index);
}

retcode_t iota_client_get_account_data(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                       uint8_t security, account_data_t* out_account) {
  retcode_t ret_code = RC_ERROR;
  address_opt_t const addr_opt = {.security = security, .start = 0, .total = 0};
  flex_trit_t* tmp_addr = NULL;
  uint64_t addr_index = 0;
  get_balances_req_t* balances_req = NULL;
  get_balances_res_t* balances_res = NULL;

  // security validation
  if (addr_opt.security == 0 || addr_opt.security > 3) {
    ret_code = RC_CCLIENT_INVALID_SECURITY;
    log_error(client_extended_logger_id, "%s %s\n", __func__, error_2_string(ret_code));
    goto done;
  }

  // get addresses
  for (addr_index = 0;; addr_index++) {
    log_debug(client_extended_logger_id, "checking address on: %" PRIu64 "\n", addr_index);
    bool is_unused_addr = false;
    tmp_addr = iota_sign_address_gen_flex_trits(seed, addr_index, addr_opt.security);
    if (tmp_addr) {
      if ((ret_code = is_unused_address(serv, tmp_addr, &is_unused_addr, true, &out_account->transactions)) != RC_OK) {
        log_error(client_extended_logger_id, "checking address status failed: %s\n", error_2_string(ret_code));
        goto done;
      }

      if (is_unused_addr) {
        // it's the latest(unused) address
        memcpy(out_account->latest_address, tmp_addr, FLEX_TRIT_SIZE_243);
        break;
      } else {
        // spent addresses
        if ((ret_code = hash243_queue_push(&out_account->addresses, tmp_addr)) != RC_OK) {
          log_error(client_extended_logger_id, "push address to list failed: %s\n", error_2_string(ret_code));
          goto done;
        }
      }
      free(tmp_addr);
      tmp_addr = NULL;
    } else {
      // gen address failed.
      ret_code = RC_CCLIENT_NULL_PTR;
      log_error(client_extended_logger_id, "%s address generation failed: %s\n", __func__, error_2_string(ret_code));
      goto done;
    }
  }

  if (out_account->addresses) {
    balances_req = get_balances_req_new();
    balances_res = get_balances_res_new();
    if (!balances_req || !balances_res) {
      ret_code = RC_OOM;
      log_error(client_extended_logger_id, "%s create get balances request or response object failed: %s\n", __func__,
                error_2_string(ret_code));
      goto done;
    }

    // get balances
    out_account->balance = 0;
    balances_req->threshold = 100;  // currently `100` should be used
    balances_req->addresses = out_account->addresses;
    if ((ret_code = iota_client_get_balances(serv, balances_req, balances_res)) == RC_OK) {
      // count all balances
      for (size_t i = 0; i < get_balances_res_balances_num(balances_res); i++) {
        uint64_t balance = get_balances_res_balances_at(balances_res, i);
        utarray_push_back(out_account->balances, &balance);
        out_account->balance += balance;
      }
    }
  }

done:
  free(tmp_addr);
  if (balances_req) {
    balances_req->addresses = NULL;  // no need to be freed
  }
  get_balances_req_free(&balances_req);
  get_balances_res_free(&balances_res);
  return ret_code;
}
