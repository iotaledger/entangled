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

bool neighbor_cmp(neighbor_t const *const lhs, neighbor_t const *const rhs) {
  if (lhs == NULL || rhs == NULL) {
    return false;
  }
  return strcmp(lhs->endpoint.host, rhs->endpoint.host) == 0 &&
         lhs->endpoint.port == rhs->endpoint.port &&
         lhs->endpoint.protocol == rhs->endpoint.protocol;
}

bool neighbor_add(neighbors_list_t *const neighbors,
                  neighbor_t const neighbor) {
  if (neighbors == NULL) {
    return false;
  }
  if (neighbors->vtable->contains(neighbors, neighbor) == true) {
    return false;
  }
  if (neighbors->vtable->push_back(neighbors, neighbor) !=
      CONCURRENT_LIST_SUCCESS) {
    return false;
  }
  return true;
}

bool neighbor_remove(neighbors_list_t *const neighbors,
                     neighbor_t const neighbor) {
  if (neighbors == NULL) {
    return false;
  }
  if (neighbors->vtable->remove(neighbors, neighbor) !=
      CONCURRENT_LIST_SUCCESS) {
    return false;
  }
  return true;
}

neighbor_t *neighbor_find_by_values(neighbors_list_t *const neighbors,
                                    protocol_type_t const protocol,
                                    char const *const host,
                                    uint16_t const port) {
  neighbor_t cmp;

  if (neighbors == NULL) {
    return false;
  }
  if (neighbor_init_with_values(&cmp, protocol, host, port) == false) {
    return NULL;
  }
  return neighbors->vtable->find(neighbors, cmp);
}
