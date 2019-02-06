/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/request/remove_neighbors.h"

remove_neighbors_req_t* remove_neighbors_req_new() {
  remove_neighbors_req_t* req =
      (remove_neighbors_req_t*)malloc(sizeof(remove_neighbors_req_t));
  if (req) {
    req->uris = NULL;
    utarray_new(req->uris, &ut_str_icd);
  }
  return req;
}

void remove_neighbors_req_add(remove_neighbors_req_t* req, const char* uri) {
  utarray_push_back(req->uris, &uri);
}

void remove_neighbors_req_free(remove_neighbors_req_t** req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->uris) {
    utarray_free((*req)->uris);
  }

  free(*req);
  *req = NULL;
}
