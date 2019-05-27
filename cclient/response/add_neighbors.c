/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/response/add_neighbors.h"

add_neighbors_res_t* add_neighbors_res_new() {
  add_neighbors_res_t* res = (add_neighbors_res_t*)malloc(sizeof(add_neighbors_res_t));
  if (res) {
    res->added_neighbors = 0;
  }
  return res;
}

void add_neighbors_res_free(add_neighbors_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  free(*res);
  *res = NULL;
}
