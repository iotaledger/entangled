/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/api/extended/get_new_address.h"
#include "cclient/api/core/find_transactions.h"
#include "cclient/api/core/were_addresses_spent_from.h"
#include "cclient/api/extended/logger.h"
#include "common/helpers/sign.h"

static retcode_t was_address_spent_from(iota_client_service_t const* const serv, flex_trit_t const* const addr,
                                        bool* const is_spent) {
  retcode_t ret_code = RC_ERROR;
  // Is it a spent address?
  were_addresses_spent_from_req_t* addr_spent_req = were_addresses_spent_from_req_new();
  were_addresses_spent_from_res_t* addr_spent_res = were_addresses_spent_from_res_new();
  if (!addr_spent_req || !addr_spent_res) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s: creating were_addresses_spent_from request or response failed: %s\n",
              __func__, error_2_string(ret_code));
    goto done;
  }

  if ((ret_code = were_addresses_spent_from_req_add(addr_spent_req, addr)) != RC_OK) {
    log_error(client_extended_logger_id, "%s: adding address to request failed.: %s\n", __func__,
              error_2_string(ret_code));
    goto done;
  }

  if ((ret_code = iota_client_were_addresses_spent_from(serv, addr_spent_req, addr_spent_res)) == RC_OK) {
    if (were_addresses_spent_from_res_states_at(addr_spent_res, 0)) {
      log_debug(client_extended_logger_id, "the address was spent from\n");
      *is_spent = true;
      goto done;
    } else {
      *is_spent = false;
    }
  } else {
    log_error(client_extended_logger_id, "Error: %s\n", error_2_string(ret_code));
    goto done;
  }

done:
  were_addresses_spent_from_req_free(&addr_spent_req);
  were_addresses_spent_from_res_free(&addr_spent_res);
  return ret_code;
}

static retcode_t find_transactions_by_address(iota_client_service_t const* const serv, flex_trit_t const* const addr,
                                              bool* const is_used, bool get_tx_list, hash243_queue_t* txs) {
  // if it's not a spent address, does it have transactions?
  retcode_t ret_code = RC_ERROR;
  size_t ret_num = 0;
  find_transactions_req_t* find_tran_req = find_transactions_req_new();
  find_transactions_res_t* find_tran_res = find_transactions_res_new();

  if (!find_tran_req || !find_tran_res) {
    ret_code = RC_OOM;
    log_error(client_extended_logger_id, "%s: creating find transactions request or response failed: %s\n", __func__,
              error_2_string(ret_code));
    goto done;
  }

  ret_code = hash243_queue_push(&find_tran_req->addresses, addr);
  if (ret_code) {
    log_error(client_extended_logger_id, "%s: hash queue push failed: %s\n", __func__, error_2_string(ret_code));
    goto done;
  }
  ret_code = iota_client_find_transactions(serv, find_tran_req, find_tran_res);
  if (ret_code == RC_OK) {
    if ((ret_num = hash243_queue_count(find_tran_res->hashes)) > 0) {
      log_debug(client_extended_logger_id, "the address has transactions\n");
      if (get_tx_list) {  // gives transactions to caller.
        log_debug(client_extended_logger_id, "cloning transactions\n");
        ret_code = hash243_queue_copy(txs, find_tran_res->hashes, ret_num);
      }
      *is_used = true;
    } else {
      log_debug(client_extended_logger_id, "the address has no transactions and was not spent from\n");
      *is_used = false;
    }
  }
done:
  find_transactions_req_free(&find_tran_req);
  find_transactions_res_free(&find_tran_res);
  return ret_code;
}
retcode_t is_used_address(iota_client_service_t const* const serv, flex_trit_t const* const addr, bool* const is_used,
                          bool with_txs, hash243_queue_t* transactions) {
  if (was_address_spent_from(serv, addr, is_used) != RC_OK) {
    log_warning(client_extended_logger_id, "were_address_spent_from failed\n");
  }
  return find_transactions_by_address(serv, addr, is_used, with_txs, transactions);
}

retcode_t iota_client_get_new_address(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                      address_opt_t const addr_opt, hash243_queue_t* out_addresses) {
  retcode_t ret = RC_ERROR;
  flex_trit_t* tmp = NULL;
  size_t addr_index = 0;
  bool is_used = false;

  log_debug(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
  // security validation
  if (addr_opt.security == 0 || addr_opt.security > 3) {
    ret = RC_CCLIENT_INVALID_SECURITY;
    log_error(client_extended_logger_id, "%s %s\n", __func__, error_2_string(ret));
    return ret;
  }

  if (addr_opt.total != 0) {  // return addresses in a list
    for (addr_index = addr_opt.start; addr_index < addr_opt.total; addr_index++) {
      tmp = iota_sign_address_gen_flex_trits(seed, addr_index, addr_opt.security);
      if (tmp) {
        ret = hash243_queue_push(out_addresses, tmp);
        if (ret) {
          log_error(client_extended_logger_id, "%s:%d hash queue push failed: %s\n", __func__, __LINE__,
                    error_2_string(ret));
          goto done;
        }
        free(tmp);
        tmp = NULL;
      } else {
        // gen address failed.
        ret = RC_OOM;
        log_error(client_extended_logger_id, "%s address generation failed: %s\n", __func__, error_2_string(ret));
        goto done;
      }
    }
  } else {  // return addresses include the latest unused address.
    for (addr_index = 0;; addr_index++) {
      tmp = iota_sign_address_gen_flex_trits(seed, addr_index, addr_opt.security);
      if (tmp) {
        ret = hash243_queue_push(out_addresses, tmp);
        if (ret) {
          log_error(client_extended_logger_id, "%s:%d hash queue push failed: %s\n", __func__, __LINE__,
                    error_2_string(ret));
          goto done;
        }
        ret = is_used_address(serv, tmp, &is_used, false, NULL);
        if (ret == RC_OK && !is_used) {
          goto done;
        }
        free(tmp);
        tmp = NULL;
      } else {
        // gen address failed.
        ret = RC_OOM;
        log_error(client_extended_logger_id, "%s address generation failed: %s\n", __func__, error_2_string(ret));
        goto done;
      }
    }
  }
done:
  free(tmp);
  return ret;
}

retcode_t iota_client_get_unspent_address(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                          address_opt_t const addr_opt, flex_trit_t* unspent_addr,
                                          uint64_t* unspent_index) {
  retcode_t ret = RC_OK;
  flex_trit_t* tmp_addr = NULL;
  uint64_t addr_index = 0;
  bool is_spent = true;

  log_debug(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
  // security validation
  if (addr_opt.security == 0 || addr_opt.security > 3) {
    ret = RC_CCLIENT_INVALID_SECURITY;
    log_error(client_extended_logger_id, "%s %s\n", __func__, error_2_string(ret));
    return ret;
  }

  for (addr_index = addr_opt.start; addr_index < addr_opt.total; addr_index++) {
    tmp_addr = iota_sign_address_gen_flex_trits(seed, addr_index, addr_opt.security);
    if (tmp_addr) {
      if (was_address_spent_from(serv, tmp_addr, &is_spent) != RC_OK) {
        log_error(client_extended_logger_id, "were_address_spent_from failed\n");
        free(tmp_addr);
        goto done;
      }

      if (!is_spent) {
        memcpy(unspent_addr, tmp_addr, NUM_FLEX_TRITS_ADDRESS);
        *unspent_index = addr_index;
        goto done;
      }

      free(tmp_addr);
      tmp_addr = NULL;
    } else {
      // gen address failed.
      ret = RC_OOM;
      log_error(client_extended_logger_id, "%s address generation failed: %s\n", __func__, error_2_string(ret));
      goto done;
    }
  }

done:
  free(tmp_addr);
  return ret;
}
