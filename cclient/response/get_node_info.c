/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/get_node_info.h"

get_node_info_res_t* get_node_info_res_new() {
  get_node_info_res_t* res =
      (get_node_info_res_t*)malloc(sizeof(get_node_info_res_t));

  if (res) {
    res->app_name = char_buffer_new();
    res->app_version = char_buffer_new();
    memset(res->latest_milestone, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
    memset(res->latest_solid_subtangle_milestone, FLEX_TRIT_NULL_VALUE,
           FLEX_TRIT_SIZE_243);
  }
  return res;
}

void get_node_info_res_free(get_node_info_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->app_name) {
    char_buffer_free((*res)->app_name);
  }
  if ((*res)->app_version) {
    char_buffer_free((*res)->app_version);
  }
  free(*res);
  *res = NULL;
}
