/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/network/lists/concurrent_list.c.inc"
#include "common/network/lists/concurrent_list_neighbor.h"

DEFINE_CONCURRENT_LIST_OF(neighbor_t);

bool cmp_neighbor(neighbor_t const *const lhs, neighbor_t const *const rhs) {
  if (lhs == NULL || rhs == NULL) {
    return false;
  }
  return lhs->endpoint.protocol == rhs->endpoint.protocol &&
         strcmp(lhs->endpoint.host, rhs->endpoint.host) == 0 &&
         lhs->endpoint.port == rhs->endpoint.port;
}

bool add_neighbor(neighbors_list_t *const neighbors,
                  neighbor_t const neighbor) {
  if (neighbors == NULL) {
    return false;
  }
  if (neighbors->vtable->contain(neighbors, neighbor) == true) {
    return false;
  }
  return neighbors->vtable->push_back(neighbors, neighbor) ==
         CONCURRENT_LIST_SUCCESS;
}

bool remove_neighbor(neighbors_list_t *const neighbors,
                     neighbor_t const neighbor) {
  if (neighbors == NULL) {
    return false;
  }
  return neighbors->vtable->remove(neighbors, neighbor) ==
         CONCURRENT_LIST_SUCCESS;
}
