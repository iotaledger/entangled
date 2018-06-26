// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_REQUEST_REMOVE_NEIGHBORS_H
#define CCLIENT_REQUEST_REMOVE_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef struct {
  /**
   * List of neighbors URIs
   */
  string_array uris;

} remove_neighbors_req_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_REMOVE_NEIGHBORS_H
