// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_REQUEST_ADD_NEIGHBORS_H
#define CCLIENT_REQUEST_ADD_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * List of neighbors URIs
   */
  trit_array_array uris;

} add_neighbors_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_FIND_TRANSACTIONS_H
