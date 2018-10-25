/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "iota_client_extended_api.h"

static retcode_t is_unused_address(iota_client_service_t const* const serv,
                                   flex_trit_t const* const addr,
                                   bool* const is_unused) {
  retcode_t ret_code = RC_OK;
  size_t ret_num = 0;
  tryte_t trytes_addr[NUM_TRYTES_ADDRESS];
  find_transactions_req_t* find_tran = NULL;
  find_transactions_res_t* res = NULL;

  find_tran = find_transactions_req_new();
  res = find_transactions_res_new();
  if (!find_tran || !res) {
    return RC_CCLIENT_NULL_PTR;
  }
  // TODO remove flex_trits-trytes convertion.
  size_t trits_num = flex_trits_to_trytes(trytes_addr, NUM_TRYTES_ADDRESS, addr,
                                          NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);
  if (trits_num) {
    find_tran =
        find_transactions_req_add_address(find_tran, (char*)trytes_addr);
    ret_code = iota_client_find_transactions(serv, find_tran, res);
    ret_num = find_transactions_res_hash_num(res);
    *is_unused = ret_num ? false : true;
  }
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

  // security validation
  if (addr_opt.security <= 0 || addr_opt.security > 3) {
    return RC_CCLIENT_INVALID_SECURITY;
  }

  if (addr_opt.total != 0) {  // return addresses in a list
    for (addr_index = addr_opt.start; addr_index < addr_opt.total;
         addr_index++) {
      tmp = iota_flex_sign_address_gen(seed, addr_index, addr_opt.security);
      if (tmp) {
        ret = hash243_queue_push(out_addresses, tmp);
        if (ret) {
          return ret;
        }
      } else {
        // gen address failed.
        return RC_CCLIENT_NULL_PTR;
      }
    }
  } else {  // return addresses include the latest unused address.
    for (addr_index = 0;; addr_index++) {
      tmp = iota_flex_sign_address_gen(seed, addr_index, addr_opt.security);
      if (tmp) {
        ret = hash243_queue_push(out_addresses, tmp);
        if (ret) {
          return ret;
        }
        ret = is_unused_address(serv, tmp, &is_unused);
        if (ret || is_unused) {
          return ret;
        }
      } else {
        // gen address failed.
        return RC_CCLIENT_NULL_PTR;
      }
    }
  }
  return ret;
}

retcode_t iota_client_get_inputs(iota_client_service_t const* const serv,
                                 flex_trit_t const* const seed,
                                 address_opt_t const addr_opt,
                                 size_t const threshold,
                                 inputs_t* const out_input) {
  retcode_t ret_code = RC_OK;

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
