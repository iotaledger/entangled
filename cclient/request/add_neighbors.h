/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_ADD_NEIGHBORS_H
#define CCLIENT_REQUEST_ADD_NEIGHBORS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* List of neighbors URIs */
typedef struct add_neighbors_req_s {
  UT_array* uris;
} add_neighbors_req_t;

add_neighbors_req_t* add_neighbors_req_new();
void add_neighbors_req_free(add_neighbors_req_t** req);

retcode_t add_neighbors_req_uris_add(add_neighbors_req_t* req, const char* uri);
const char* add_neighbors_req_uris_at(add_neighbors_req_t* req, size_t idx);
size_t add_neighbors_req_uris_len(add_neighbors_req_t* req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_ADD_NEIGHBORS_H
