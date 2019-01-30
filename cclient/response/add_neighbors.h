/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_ADD_NEIGHBORS_H
#define CCLIENT_RESPONSE_ADD_NEIGHBORS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// response is a number of neighbors.
typedef struct add_neighbors_res_s {
  int added_neighbors;
} add_neighbors_res_t;

add_neighbors_res_t* add_neighbors_res_new();
void add_neighbors_res_free(add_neighbors_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_ADD_NEIGHBORS_H
