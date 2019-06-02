/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/response/remove_neighbors.h"

remove_neighbors_res_t* remove_neighbors_res_new() {
  remove_neighbors_res_t* res = (remove_neighbors_res_t*)malloc(sizeof(remove_neighbors_res_t));
  if (res) {
    res->removed_neighbors = 0;
  }
  return res;
}

void remove_neighbors_res_free(remove_neighbors_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  free(*res);
  *res = NULL;
}
