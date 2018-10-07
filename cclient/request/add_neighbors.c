/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/add_neighbors.h"

add_neighbors_req_t* add_neighbors_req_new() {
  add_neighbors_req_t* uris = NULL;
  utarray_new(uris, &ut_str_icd);
  return uris;
}

void add_neighbors_req_add(add_neighbors_req_t* uris, const char* uri) {
  utarray_push_back(uris, &uri);
}

void add_neighbors_req_free(add_neighbors_req_t* ut) { utarray_free(ut); }