/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_REMOVE_NEIGHBORS_H
#define CCLIENT_RESPONSE_REMOVE_NEIGHBORS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

// response is a number of neighbors.
typedef struct remove_neighbors_res_s {
  int removed_neighbors;
} remove_neighbors_res_t;

remove_neighbors_res_t* remove_neighbors_res_new();
void remove_neighbors_res_free(remove_neighbors_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_REMOVE_NEIGHBORS_H
