/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/attach_to_tangle.h"

attach_to_tangle_res_t* attach_to_tangle_res_new() {
  attach_to_tangle_res_t* res =
      (attach_to_tangle_res_t*)malloc(sizeof(attach_to_tangle_res_t));
  if (res) {
    res->trytes = NULL;
  }
  return res;
}

flex_trit_t* attach_to_tangle_res_trytes_at(attach_to_tangle_res_t* res,
                                            int index) {
  return hash8019_queue_at(&res->trytes, index);
}

size_t attach_to_tangle_res_trytes_cnt(attach_to_tangle_res_t* res) {
  return hash8019_queue_count(&res->trytes);
}

void attach_to_tangle_res_free(attach_to_tangle_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->trytes) {
    hash8019_queue_free(&(*res)->trytes);
  }

  free(*res);
  *res = NULL;
}
