/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_NETWORK_ROUTER_H__
#define __CIRI_NODE_NETWORK_ROUTER_H__

#include "utarray.h"

#include "ciri/node/conf.h"
#include "ciri/node/network/neighbor.h"
#include "utils/handles/rw_lock.h"

#define NEIGHBORS_FOREACH(neighbors, neighbor)                              \
  for (neighbor = (neighbor_t *)utarray_front(neighbors); neighbor != NULL; \
       neighbor = (neighbor_t *)utarray_next(neighbors, neighbor))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct router_s {
  iota_node_conf_t *conf;
  UT_array *neighbors;
  rw_lock_handle_t neighbors_lock;
} router_t;

/**
 * Initializes a router
 *
 * @param router The router
 * @param conf A node configuration
 *
 * @return a status code
 */
retcode_t router_init(router_t *const router, iota_node_conf_t *const conf);

/**
 * Destroys a router
 *
 * @param router The router
 *
 * @return a status code
 */
retcode_t router_destroy(router_t *const router);

/**
 * Adds a neighbor to a router
 *
 * @param Router The router
 * @param neighbor The neighbor
 *
 * @return a status code
 */
retcode_t router_neighbor_add(router_t *const router, neighbor_t const *const neighbor);

/**
 * Removes a neighbor from a router
 *
 * @param Router The router
 * @pram neighbor The neighbor
 *
 * @return a status code
 */
retcode_t router_neighbor_remove(router_t *const router, neighbor_t const *const neighbor);

/**
 * Gives the number of neighbors in a router
 *
 * @param Router The router
 *
 * @return the number of neighbors in the router
 */
size_t router_neighbors_count(router_t *const router);

/**
 * Finds a neigbor matching given endpoint in a router
 *
 * @param router The router
 * @param endpoint The endpoint
 *
 * @return a pointer to the neigbor if found, NULL otherwise
 */
neighbor_t *router_neighbor_find_by_endpoint(router_t *const router, endpoint_t const *const endpoint);

/**
 * Finds a neigbor matching given endpoint values in a router
 *
 * @param router The router
 * @param ip The endpoint ip
 * @param port The endpoint port
 *
 * @return a pointer to the neigbor if found, NULL otherwise
 */
neighbor_t *router_neighbor_find_by_endpoint_values(router_t *const router, char const *const ip, uint16_t const port);

#ifdef __cplusplus
}
#endif

#endif
