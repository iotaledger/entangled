/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/api/core/find_transactions.h"
#include "cclient/api/core/get_balances.h"
#include "common/helpers/sign.h"

#include "cclient/api/extended/get_account_data.h"
#include "cclient/api/extended/logger.h"

retcode_t iota_client_get_account_data(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                       size_t const security, account_data_t* out_account) {
  retcode_t ret_code = RC_OK;
  address_opt_t const addr_opt = {.security = security, .start = 0, .total = 0};
  flex_trit_t* tmp_addr = NULL;
  size_t addr_index = 0;
  hash243_queue_entry_t* tx_iter = NULL;
  find_transactions_req_t* find_tx_req = NULL;
  find_transactions_res_t* find_tx_res = NULL;
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
    tmp_addr = iota_sign_address_gen_flex_trits(seed, addr_index, addr_opt.security);
    if (tmp_addr) {
      // check tx
      size_t tx_num = 0;
      find_tx_req = find_transactions_req_new();
      find_tx_res = find_transactions_res_new();
      if (!find_tx_req || !find_tx_res) {
        ret_code = RC_CCLIENT_OOM;
        log_error(client_extended_logger_id, "%s find transactions request or response object failed: %s\n", __func__,
                  error_2_string(ret_code));
        goto done;
      }
      ret_code = hash243_queue_push(&find_tx_req->addresses, tmp_addr);
      if (ret_code) {
        log_error(client_extended_logger_id, "%s hash243_queue_push failed: %s\n", __func__, error_2_string(ret_code));
        goto done;
      }

      ret_code = iota_client_find_transactions(serv, find_tx_req, find_tx_res);
      if (!ret_code) {
        tx_num = hash243_queue_count(find_tx_res->hashes);
        if (tx_num) {
          // appending address
          ret_code = hash243_queue_push(&out_account->addresses, tmp_addr);
          if (ret_code) {
            log_error(client_extended_logger_id, "%s hash243_queue_push failed: %s\n", __func__,
                      error_2_string(ret_code));
            goto done;
          }

          // appending tx
          tx_iter = NULL;
          CDL_FOREACH(find_tx_res->hashes, tx_iter) {
            ret_code = hash243_queue_push(&out_account->transactions, tx_iter->hash);
            if (ret_code) {
              log_error(client_extended_logger_id, "%s hash243_queue_push failed: %s\n", __func__,
                        error_2_string(ret_code));
              goto done;
            }
          }
          free(tmp_addr);
        } else {
          // it's the latest address
          memcpy(out_account->latest_address, tmp_addr, FLEX_TRIT_SIZE_243);
          break;
        }
      }

      find_transactions_req_free(&find_tx_req);
      find_transactions_res_free(&find_tx_res);
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
      ret_code = RC_CCLIENT_OOM;
      log_error(client_extended_logger_id, "%s create get balances request or response object failed: %s\n", __func__,
                error_2_string(ret_code));
      goto done;
    }

    // get balances
    out_account->balance = 0;
    balances_req->threshold = 100;  // currently `100` should be used
    balances_req->addresses = out_account->addresses;
    ret_code = iota_client_get_balances(serv, balances_req, balances_res);
    if (!ret_code) {
      // count all balances
      for (int i = 0; i < get_balances_res_balances_num(balances_res); i++) {
        out_account->balance += get_balances_res_balances_at(balances_res, i);
      }
    }
  }

done:
  free(tmp_addr);
  find_transactions_req_free(&find_tx_req);
  find_transactions_res_free(&find_tx_res);
  balances_req->addresses = NULL;  // no need to be freed
  get_balances_req_free(&balances_req);
  get_balances_res_free(&balances_res);
  return ret_code;
}
