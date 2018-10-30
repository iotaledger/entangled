/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "iota_client_extended_api.h"

#define CCLIENT_EXTENDED_LOGGER_ID "cclient_extended_api"

void iota_client_extended_init() {
  logger_helper_init(CCLIENT_EXTENDED_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(CCLIENT_EXTENDED_LOGGER_ID, "[%s:%d] enable logger %s.\n", __func__,
           __LINE__, CCLIENT_EXTENDED_LOGGER_ID);
}

void iota_client_extended_destroy() {
  log_info(CCLIENT_EXTENDED_LOGGER_ID, "[%s:%d] destroy logger %s.\n", __func__,
           __LINE__, CCLIENT_EXTENDED_LOGGER_ID);
  logger_helper_destroy(CCLIENT_EXTENDED_LOGGER_ID);
}

static retcode_t is_unused_address(iota_client_service_t const* const serv,
                                   flex_trit_t const* const addr,
                                   bool* const is_unused) {
  retcode_t ret_code = RC_OK;
  size_t ret_num = 0;
  find_transactions_req_t* find_tran_req = NULL;
  find_transactions_res_t* find_tran_res = NULL;

  log_debug(CCLIENT_EXTENDED_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  find_tran_req = find_transactions_req_new();
  find_tran_res = find_transactions_res_new();
  if (!find_tran_req || !find_tran_res) {
    ret_code = RC_CCLIENT_NULL_PTR;
    goto done;
  }

  ret_code = hash243_queue_push(&find_tran_req->addresses, addr);
  if (ret_code) {
    goto done;
  }
  ret_code = iota_client_find_transactions(serv, find_tran_req, find_tran_res);
  if (!ret_code) {
    ret_num = hash243_queue_count(&find_tran_res->hashes);
    *is_unused = ret_num ? false : true;
  }
done:
  find_transactions_req_free(&find_tran_req);
  find_transactions_res_free(&find_tran_res);
  return ret_code;
}

retcode_t iota_client_get_new_address(iota_client_service_t const* const serv,
                                      flex_trit_t const* const seed,
                                      address_opt_t const addr_opt,
                                      hash243_queue_t* out_addresses) {
  retcode_t ret = RC_OK;
  flex_trit_t* tmp = NULL;
  size_t addr_index = 0;
  bool is_unused = true;

  log_debug(CCLIENT_EXTENDED_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  // security validation
  if (addr_opt.security == 0 || addr_opt.security > 3) {
    return RC_CCLIENT_INVALID_SECURITY;
  }

  if (addr_opt.total != 0) {  // return addresses in a list
    for (addr_index = addr_opt.start; addr_index < addr_opt.total;
         addr_index++) {
      tmp = iota_flex_sign_address_gen(seed, addr_index, addr_opt.security);
      if (tmp) {
        ret = hash243_queue_push(out_addresses, tmp);
        if (ret) {
          goto done;
        }
        free(tmp);
      } else {
        // gen address failed.
        ret = RC_CCLIENT_NULL_PTR;
        goto done;
      }
    }
  } else {  // return addresses include the latest unused address.
    for (addr_index = 0;; addr_index++) {
      tmp = iota_flex_sign_address_gen(seed, addr_index, addr_opt.security);
      if (tmp) {
        ret = hash243_queue_push(out_addresses, tmp);
        if (ret) {
          goto done;
        }
        ret = is_unused_address(serv, tmp, &is_unused);
        if (ret || is_unused) {
          goto done;
        }
        free(tmp);
      } else {
        // gen address failed.
        ret = RC_CCLIENT_NULL_PTR;
        goto done;
      }
    }
  }
done:
  if (tmp) {
    free(tmp);
  }
  return ret;
}

retcode_t iota_client_get_inputs(iota_client_service_t const* const serv,
                                 flex_trit_t const* const seed,
                                 address_opt_t const addr_opt,
                                 size_t const threshold,
                                 inputs_t* const out_input) {
  retcode_t ret_code = RC_OK;

  log_debug(CCLIENT_EXTENDED_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  // get address list
  ret_code =
      iota_client_get_new_address(serv, seed, addr_opt, &out_input->addresses);
  if (ret_code) {
    return ret_code;
  }

  // get balances from all addresses
  get_balances_req_t* blances_req = get_balances_req_new();
  get_balances_res_t* blances_res = get_balances_res_new();
  blances_req->threshold = threshold;
  // assign the list directly, not making a copy.
  blances_req->addresses = out_input->addresses;

  ret_code = iota_client_get_balances(serv, blances_req, blances_res);
  out_input->total_balance = get_balances_res_total_balance(blances_res);

  blances_req->addresses = NULL;  // no need to be freed
  get_balances_req_free(&blances_req);
  get_balances_res_free(&blances_res);
  return ret_code;
}

retcode_t iota_client_get_account_data(iota_client_service_t const* const serv,
                                       flex_trit_t const* const seed,
                                       size_t const security,
                                       account_data_t* out_account) {
  retcode_t ret_code = RC_OK;
  address_opt_t const addr_opt = {.security = security, .start = 0, .total = 0};
  flex_trit_t* tmp_addr = NULL;
  size_t addr_index = 0;
  hash243_queue_entry_t* tx_iter = NULL;
  find_transactions_req_t* find_tx_req = NULL;
  find_transactions_res_t* find_tx_res = NULL;
  get_balances_req_t* blances_req = get_balances_req_new();
  get_balances_res_t* blances_res = get_balances_res_new();

  // security validation
  if (addr_opt.security == 0 || addr_opt.security > 3) {
    ret_code = RC_CCLIENT_INVALID_SECURITY;
    goto done;
  }

  // get addresses
  for (addr_index = 0;; addr_index++) {
    tmp_addr = iota_flex_sign_address_gen(seed, addr_index, addr_opt.security);
    if (tmp_addr) {
      // check tx
      size_t tx_num = 0;
      find_tx_req = find_transactions_req_new();
      find_tx_res = find_transactions_res_new();
      if (!find_tx_req || !find_tx_res) {
        ret_code = RC_CCLIENT_NULL_PTR;
        goto done;
      }
      ret_code = hash243_queue_push(&find_tx_req->addresses, tmp_addr);
      if (ret_code) {
        goto done;
      }

      ret_code = iota_client_find_transactions(serv, find_tx_req, find_tx_res);
      if (!ret_code) {
        tx_num = hash243_queue_count(&find_tx_res->hashes);
        if (tx_num) {
          // appending address
          ret_code = hash243_queue_push(&out_account->addresses, tmp_addr);
          if (ret_code) {
            goto done;
          }

          // appending tx
          tx_iter = NULL;
          CDL_FOREACH(find_tx_res->hashes, tx_iter) {
            ret_code =
                hash243_queue_push(&out_account->transactions, tx_iter->hash);
            if (ret_code) {
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
      goto done;
    }
  }

  if (out_account->addresses) {
    // get balances
    blances_req->threshold = 100;
    blances_req->addresses = out_account->addresses;
    ret_code = iota_client_get_balances(serv, blances_req, blances_res);
    if (!ret_code) {
      out_account->balance = get_balances_res_total_balance(blances_res);
    }
  }

done:
  if (tmp_addr) {
    free(tmp_addr);
  }
  find_transactions_req_free(&find_tx_req);
  find_transactions_res_free(&find_tx_res);
  blances_req->addresses = NULL;  // no need to be freed
  get_balances_req_free(&blances_req);
  get_balances_res_free(&blances_res);
  return ret_code;
}
