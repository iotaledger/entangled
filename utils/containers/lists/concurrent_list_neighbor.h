/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_NEIGHBOR_H__
#define __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_NEIGHBOR_H__

#include "gossip/neighbor.h"
#include "utils/containers/lists/concurrent_list.h.inc"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_CL(neighbor_t);

typedef concurrent_list_neighbor_t neighbors_list_t;

/**
 * neighbor_t comparator for list operations
 *
 * @param lhs Left hand side neighbor_t
 * @param rhs Right hand side neighbor_t
 *
 * @return true if equal, false otherwise
 */
bool neighbor_cmp(neighbor_t const *const lhs, neighbor_t const *const rhs);

/**
 * Add a neighbor to a list
 *
 * @param neighbors The neighbors list
 * @param neighbor The neighbor
 *
 * @return true if the neighbor was added, false otherwise
 */
bool neighbor_add(neighbors_list_t *const neighbors, neighbor_t const neighbor);

/**
 * Remove a neighbor from a list
 *
 * @param neighbors The neighbors list
 * @pram neighbor The neighbor
 *
 * @return true if the neighbor was removed, false otherwise
 */
bool neighbor_remove(neighbors_list_t *const neighbors,
                     neighbor_t const neighbor);

/**
 * Find a neigbor matching given endpoint
 *
 * @param neighbors The neighbors list
 * @param endpoint The endpoint
 *
 * @return a pointer to the neigbor if found, NULL otherwise
 */
neighbor_t *neighbor_find_by_endpoint(neighbors_list_t *const neighbors,
                                      endpoint_t const *const endpoint);

/**
 * Find a neigbor matching given endpoint values
 *
 * @param neighbors The neighbors list
 * @param ip The endpoint ip
 * @param port The endpoint port
 * @param protocol The endpoint protocol
 *
 * @return a pointer to the neigbor if found, NULL otherwise
 */
neighbor_t *neighbor_find_by_endpoint_values(neighbors_list_t *const neighbors,
                                             char const *const ip,
                                             uint16_t const port,
                                             protocol_type_t const protocol);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_NEIGHBOR_H__
