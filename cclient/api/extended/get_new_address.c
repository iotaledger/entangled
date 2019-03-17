/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/api/core/find_transactions.h"
#include "common/helpers/sign.h"

#include "cclient/api/extended/get_new_address.h"
#include "cclient/api/extended/logger.h"

static retcode_t is_unused_address(iota_client_service_t const* const serv, flex_trit_t const* const addr,
                                   bool* const is_unused) {
  retcode_t ret_code = RC_OK;
  size_t ret_num = 0;
  find_transactions_req_t* find_tran_req = NULL;
  find_transactions_res_t* find_tran_res = NULL;

  log_info(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
  find_tran_req = find_transactions_req_new();
  find_tran_res = find_transactions_res_new();
  if (!find_tran_req || !find_tran_res) {
    ret_code = RC_CCLIENT_OOM;
    log_error(client_extended_logger_id, "%s: create find transactions request or response failed: %s\n", __func__,
              error_2_string(ret_code));
    goto done;
  }

  ret_code = hash243_queue_push(&find_tran_req->addresses, addr);
  if (ret_code) {
    log_error(client_extended_logger_id, "%s: hahs queue push failed: %s\n", __func__, error_2_string(ret_code));
    goto done;
  }
  ret_code = iota_client_find_transactions(serv, find_tran_req, find_tran_res);
  if (ret_code == RC_OK) {
    ret_num = hash243_queue_count(find_tran_res->hashes);
    *is_unused = ret_num ? false : true;
  }
done:
  find_transactions_req_free(&find_tran_req);
  find_transactions_res_free(&find_tran_res);
  return ret_code;
}

retcode_t iota_client_get_new_address(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                      address_opt_t const addr_opt, hash243_queue_t* out_addresses) {
  retcode_t ret = RC_OK;
  flex_trit_t* tmp = NULL;
  size_t addr_index = 0;
  bool is_unused = true;

  log_info(client_extended_logger_id, "[%s:%d]\n", __func__, __LINE__);
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
        ret = RC_CCLIENT_OOM;
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
        ret = is_unused_address(serv, tmp, &is_unused);
        if (ret == RC_OK && is_unused) {
          goto done;
        }
        free(tmp);
        tmp = NULL;
      } else {
        // gen address failed.
        ret = RC_CCLIENT_OOM;
        log_error(client_extended_logger_id, "%s address generation failed: %s\n", __func__, error_2_string(ret));
        goto done;
      }
    }
  }
done:
  free(tmp);
  return ret;
}
