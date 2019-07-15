/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_NETWORK_ROUTER_H__
#define __CIRI_NODE_NETWORK_ROUTER_H__

#include "utarray.h"

#include "ciri/node/network/neighbor.h"
#include "utils/handles/rw_lock.h"

#define NEIGHBORS_FOREACH(neighbors, neighbor)                              \
  for (neighbor = (neighbor_t *)utarray_front(neighbors); neighbor != NULL; \
       neighbor = (neighbor_t *)utarray_next(neighbors, neighbor))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct router_s {
  node_t *node;
  UT_array *neighbors;
  rw_lock_handle_t neighbors_lock;
} router_t;

/**
 * Initializes a router
 *
 * @param[in,out] router  The router
 * @param[in]     node    A node
 *
 * @return a status code
 */
retcode_t router_init(router_t *const router, node_t *const node);

/**
 * Destroys a router
 *
 * @param[in,out] router  The router
 *
 * @return a status code
 */
retcode_t router_destroy(router_t *const router);

/**
 * Adds a neighbor to a router
 *
 * @param[in,out] router    The router
 * @param[in,out] neighbor  The neighbor
 *
 * @return a status code
 */
retcode_t router_neighbor_add(router_t *const router, neighbor_t *const neighbor);

/**
 * Removes a neighbor from a router
 *
 * @param[in,out] router    The router
 * @param[in]     neighbor  The neighbor
 *
 * @return a status code
 */
retcode_t router_neighbor_remove(router_t *const router, neighbor_t const *const neighbor);

/**
 * Gives the number of neighbors in a router
 *
 * @param Router[in]  router  The router
 *
 * @return the number of neighbors in the router
 */
size_t router_neighbors_count(router_t *const router);

/**
 * Finds a neighbor matching given endpoint in a router
 *
 * @param[in,out] router    The router
 * @param[in]     endpoint  The endpoint
 *
 * @return a pointer to the neighbor if found, NULL otherwise
 */
neighbor_t *router_neighbor_find_by_endpoint(router_t *const router, endpoint_t const *const endpoint);

/**
 * Finds a neighbor matching given endpoint values in a router
 *
 * @param[in,out] router  The router
 * @param[in]     ip      The endpoint ip
 * @param[in]     port    The endpoint port
 *
 * @return a pointer to the neighbor if found, NULL otherwise
 */
neighbor_t *router_neighbor_find_by_endpoint_values(router_t *const router, char const *const ip, uint16_t const port);

/**
 * Reads and validate a handshake and associate it with a neighbor
 *
 * @param[in,out] router    The router
 * @param[in]     ip        The IP
 * @param[in]     port      The port
 * @param[in]     buf       The buffer
 * @param[in]     nread     The buffer size
 * @param[in,out] neighbor  The neighbor
 *
 * @return a pointer to the neighbor if found, NULL otherwise
 */
retcode_t router_neighbor_read_handshake(router_t *const router, char const *const ip, uint16_t const port,
                                         void const *const buf, size_t const nread, neighbor_t **const neighbor);

/**
 * Reads a buffer from a neighbor
 *
 * @param[in,out] router      The router
 * @param[in]     neighbor    The neighbor
 * @param[in]     buffer      The buffer
 * @param[in]     buffer_size The buffer size
 *
 * @return a status code
 */
retcode_t router_neighbor_read(router_t *const router, neighbor_t *const neighbor, void const *const buffer,
                               size_t const buffer_size);

/**
 * Attempts to reconnect all disconnected neighbors
 *
 * @param[in,out] router  The router
 *
 * @return a status code
 */
retcode_t router_neighbors_reconnect_attempt(router_t *const router);

#ifdef __cplusplus
}
#endif

#endif
