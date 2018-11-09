/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "utils/containers/lists/concurrent_list.c.inc"
#include "utils/containers/lists/concurrent_list_neighbor.h"

DEFINE_CL(neighbor_t);

bool neighbor_cmp(neighbor_t const *const lhs, neighbor_t const *const rhs) {
  if (lhs == NULL || rhs == NULL) {
    return false;
  }
  return strcmp(lhs->endpoint.ip, rhs->endpoint.ip) == 0 &&
         lhs->endpoint.port == rhs->endpoint.port &&
         lhs->endpoint.protocol == rhs->endpoint.protocol;
}

bool neighbor_add(neighbors_list_t *const neighbors,
                  neighbor_t const neighbor) {
  if (neighbors == NULL) {
    return false;
  }
  if (CL_CONTAINS(neighbors, neighbor) == true) {
    return false;
  }
  if (CL_PUSH_BACK(neighbors, neighbor) != CL_SUCCESS) {
    return false;
  }
  return true;
}

bool neighbor_remove(neighbors_list_t *const neighbors,
                     neighbor_t const neighbor) {
  if (neighbors == NULL) {
    return false;
  }
  if (CL_REMOVE(neighbors, neighbor) != CL_SUCCESS) {
    return false;
  }
  return true;
}

neighbor_t *neighbor_find_by_endpoint(neighbors_list_t *const neighbors,
                                      endpoint_t const *const endpoint) {
  if (neighbors == NULL || endpoint == NULL) {
    return NULL;
  }
  return neighbor_find_by_endpoint_values(neighbors, endpoint->ip,
                                          endpoint->port, endpoint->protocol);
}

neighbor_t *neighbor_find_by_endpoint_values(neighbors_list_t *const neighbors,
                                             char const *const ip,
                                             uint16_t const port,
                                             protocol_type_t const protocol) {
  neighbor_t cmp;

  if (neighbors == NULL) {
    return NULL;
  }
  if (neighbor_init_with_values(&cmp, ip, port, protocol)) {
    return NULL;
  }
  return CL_FIND(neighbors, cmp);
}
