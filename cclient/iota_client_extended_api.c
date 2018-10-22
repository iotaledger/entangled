/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "iota_client_extended_api.h"

static bool is_unused_address(iota_client_service_t const* const serv,
                              flex_trit_t const* const addr) {
  // retcode_t ret = RC_OK;
  int ret_num = 0;
  tryte_t trytes_addr[NUM_TRYTES_ADDRESS];
  find_transactions_req_t* find_tran = find_transactions_req_new();
  find_transactions_res_t* res = find_transactions_res_new();

  // TODO remove flex_trits-trytes convertion.
  flex_trits_to_trytes(trytes_addr, NUM_TRYTES_ADDRESS, addr, NUM_TRITS_ADDRESS,
                       NUM_TRITS_ADDRESS);
  find_tran = find_transactions_req_add_address(find_tran, (char*)trytes_addr);

  iota_client_find_transactions(serv, find_tran, &res);

  ret_num = find_transactions_res_hash_num(res);
  return ret_num ? true : false;
}

retcode_t iota_client_get_new_address(iota_client_service_t const* const serv,
                                      flex_trit_t const* const seed,
                                      address_opt_t const addr_opt,
                                      hash_queue_t* out_addresses) {
  retcode_t ret = RC_OK;
  flex_trit_t* tmp = NULL;
  size_t addr_index = 0;

  // security validation
  if (addr_opt.security <= 0 || addr_opt.security > 3) {
    return RC_CCLIENT_INVALID_SECURITY;
  }

  if (addr_opt.total != 0) {  // return addresses in a list
    for (addr_index = addr_opt.start; addr_index < addr_opt.total;
         addr_index++) {
      tmp = iota_flex_sign_address_gen(seed, addr_index, addr_opt.security);
      if (tmp) {
        ret = hash_queue_push(out_addresses, tmp);
        if (ret) {
          return ret;
        }
      } else {
        // gen address failed.
        return RC_CCLIENT_NULL_PTR;
      }
    }
  } else {  // return an unused address
    for (addr_index = 0;; addr_index++) {
      tmp = iota_flex_sign_address_gen(seed, addr_index, addr_opt.security);
      if (tmp) {
        if (!is_unused_address(serv, tmp)) {
          ret = hash_queue_push(out_addresses, tmp);
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
