/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/get_trytes.h"

get_trytes_res_t* get_trytes_res_new() {
  get_trytes_res_t* res = (get_trytes_res_t*)malloc(sizeof(get_trytes_res_t));
  if (res) {
    res->trytes = NULL;
  }
  return res;
}

void get_trytes_res_free(get_trytes_res_t** const res) {
  if (!res || !(*res)) {
    return;
  }

  get_trytes_res_t* tmp = *res;

  if (tmp->trytes) {
    hash8019_queue_free(&tmp->trytes);
  }
  free(tmp);
  *res = NULL;
}
