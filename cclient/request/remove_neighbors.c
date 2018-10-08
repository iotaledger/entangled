/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/remove_neighbors.h"

remove_neighbors_req_t* remove_neighbors_req_new() {
  remove_neighbors_req_t* uris = NULL;
  utarray_new(uris, &ut_str_icd);
  return uris;
}

void remove_neighbors_req_add(remove_neighbors_req_t* uris, const char* uri) {
  utarray_push_back(uris, &uri);
}

void remove_neighbors_req_free(remove_neighbors_req_t* ut) { utarray_free(ut); }