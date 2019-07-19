/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/request/remove_neighbors.h"

remove_neighbors_req_t* remove_neighbors_req_new() {
  remove_neighbors_req_t* req = (remove_neighbors_req_t*)malloc(sizeof(remove_neighbors_req_t));
  if (req) {
    req->uris = NULL;
    utarray_new(req->uris, &ut_str_icd);
  }
  return req;
}

retcode_t remove_neighbors_req_add(remove_neighbors_req_t* req, char const* uri) {
  if (!req || !uri) {
    return RC_NULL_PARAM;
  }

  if (!req->uris) {
    utarray_new(req->uris, &ut_str_icd);
  }
  if (!req->uris) {
    return RC_OOM;
  }
  utarray_push_back(req->uris, &uri);
  return RC_OK;
}

size_t remove_neighbors_req_uris_len(remove_neighbors_req_t* req) {
  if (!req) {
    return 0;
  }
  return utarray_len(req->uris);
}

char const* remove_neighbors_req_uris_at(remove_neighbors_req_t* req, size_t idx) {
  if (!req) {
    return NULL;
  }
  return *(char const**)utarray_eltptr(req->uris, idx);
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
