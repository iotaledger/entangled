/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/get_tips.h"

get_tips_res_t* get_tips_res_new() {
  get_tips_res_t* res = (get_tips_res_t*)malloc(sizeof(get_tips_res_t));
  if (res) {
    res->hashes = NULL;
  }
  return res;
}

flex_trit_t* get_tips_res_hash_at(get_tips_res_t* res, size_t index) {
  return hash243_queue_at(&res->hashes, index);
}

size_t get_tips_res_hash_num(get_tips_res_t* res) {
  return hash243_queue_count(&res->hashes);
}

void get_tips_res_free(get_tips_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->hashes) {
    hash243_queue_free(&(*res)->hashes);
  }
  free(*res);
  *res = NULL;
}
