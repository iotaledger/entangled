/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/response/attach_to_tangle.h"

attach_to_tangle_res_t* attach_to_tangle_res_new() {
  attach_to_tangle_res_t* res = (attach_to_tangle_res_t*)malloc(sizeof(attach_to_tangle_res_t));
  if (res) {
    res->trytes = hash8019_array_new();
  }
  return res;
}

retcode_t attach_to_tangle_res_trytes_add(attach_to_tangle_res_t* res, flex_trit_t const* const trytes) {
  if (!res->trytes) {
    res->trytes = hash8019_array_new();
  }
  if (!res->trytes) {
    return RC_OOM;
  }
  hash_array_push(res->trytes, trytes);
  return RC_OK;
}

flex_trit_t* attach_to_tangle_res_trytes_at(attach_to_tangle_res_t* res, int index) {
  return hash_array_at(res->trytes, index);
}

size_t attach_to_tangle_res_trytes_cnt(attach_to_tangle_res_t* res) {
  if (!res->trytes) {
    return 0;
  }
  return hash_array_len(res->trytes);
}

void attach_to_tangle_res_free(attach_to_tangle_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->trytes) {
    hash_array_free((*res)->trytes);
  }

  free(*res);
  *res = NULL;
}
