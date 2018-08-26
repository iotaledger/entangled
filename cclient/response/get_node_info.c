/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_node_info.h"

get_node_info_res_t* get_node_info_res_new() {
  get_node_info_res_t* res =
      (get_node_info_res_t*)malloc(sizeof(get_node_info_res_t));

  res->appName = char_buffer_new();
  res->appVersion = char_buffer_new();
  res->latestMilestone = char_buffer_new();
  res->latestSolidSubtangleMilestone = char_buffer_new();

  return res;
}

void get_node_info_res_free(get_node_info_res_t* res) {
  if (res) {
    char_buffer_free(res->appName);
    char_buffer_free(res->appVersion);
    char_buffer_free(res->latestMilestone);
    char_buffer_free(res->latestSolidSubtangleMilestone);
    free(res);
    res = NULL;
  }
}
