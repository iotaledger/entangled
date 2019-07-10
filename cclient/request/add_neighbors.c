/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/request/add_neighbors.h"

add_neighbors_req_t* add_neighbors_req_new() {
  add_neighbors_req_t* req = (add_neighbors_req_t*)malloc(sizeof(add_neighbors_req_t));
  if (req) {
    req->uris = NULL;
    utarray_new(req->uris, &ut_str_icd);
  }
  return req;
}

retcode_t add_neighbors_req_uris_add(add_neighbors_req_t* req, char const* const uri) {
  if (!req->uris) {
    utarray_new(req->uris, &ut_str_icd);
  }
  if (!req->uris) {
    return RC_OOM;
  }

  utarray_push_back(req->uris, &uri);
  return RC_OK;
}

size_t add_neighbors_req_uris_len(add_neighbors_req_t* req) {
  if (!req->uris) {
    return 0;
  }
  return utarray_len(req->uris);
}

const char* add_neighbors_req_uris_at(add_neighbors_req_t* req, size_t idx) {
  if (!req->uris) {
    return NULL;
  }

  return *(const char**)utarray_eltptr(req->uris, idx);
}

void add_neighbors_req_free(add_neighbors_req_t** req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->uris) {
    utarray_free((*req)->uris);
  }

  free(*req);
  *req = NULL;
}
