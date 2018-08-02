/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_CONCURRENT_LIST_NEIGHBOR_H__
#define __COMMON_NETWORK_CONCURRENT_LIST_NEIGHBOR_H__

#include "common/network/lists/concurrent_list.h.inc"
#include "common/network/neighbor.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_CONCURRENT_LIST_OF(neighbor_t);

typedef concurrent_list_of_neighbor_t neighbors_list_t;

bool neighbor_cmp(neighbor_t const *const lhs, neighbor_t const *const rhs);
bool neighbor_add(neighbors_list_t *const neighbors, neighbor_t const neighbor);
bool neighbor_remove(neighbors_list_t *const neighbors,
                     neighbor_t const neighbor);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_CONCURRENT_LIST_NEIGHBOR_H__
