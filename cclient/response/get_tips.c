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
    res->tips = flex_hash_array_new();
  }
  return res;
}

void get_tips_res_free(get_tips_res_t* const res) {
  if (!res) {
    return;
  }

  if (res->tips) {
    flex_hash_array_free(res->tips);
  }
  free(res);
}

get_tips_res_t* get_tips_res_add_tip(get_tips_res_t* const res,
                                     tryte_t const* const tip) {
  res->tips = flex_hash_array_append(res->tips, tip);
  return res;
}

trit_array_p get_tips_res_tip_at(get_tips_res_t* const res,
                                 size_t const index) {
  return flex_hash_array_at(res->tips, index);
}

size_t get_tips_res_tip_num(get_tips_res_t* const res) {
  return flex_hash_array_count(res->tips);
}
