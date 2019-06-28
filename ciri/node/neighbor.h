/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_NEIGHBOR_H__
#define __NODE_NEIGHBOR_H__

#include <stdbool.h>

#include "utlist.h"

#include "ciri/node/iota_packet.h"
#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utarray.h"

// Forward declarations
typedef struct node_s node_t;
typedef struct tangle_s tangle_t;

typedef struct neighbor_s {
  endpoint_t endpoint;
  uint64_t nbr_all_txs;
  uint64_t nbr_new_txs;
  uint64_t nbr_invalid_txs;
  uint64_t nbr_stale_txs;
  uint64_t nbr_sent_txs;
  uint64_t nbr_random_tx_reqs;
  struct neighbor_s *next;
} neighbor_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a neighbor with an URI
 *
 * @param neighbor The neighbor
 * @param uri The URI (e.g udp://8.8.8.8:14265)
 *
 * @return a status code
 */
retcode_t neighbor_init_with_uri(neighbor_t *const neighbor, char const *const uri);

/**
 * Initializes a neighbor with endpoint values
 *
 * @param neighbor The neighbor
 * @param ip The endpoint ip
 * @param port The endpoint port
 * @param protocol The endpoint protocol
 *
 * @return a status code
 */
retcode_t neighbor_init_with_values(neighbor_t *const neighbor, char const *const ip, uint16_t const port,
                                    protocol_type_t const protocol);

/**
 * Sends a packet to a neighbor
 *
 * @param node A node
 * @param neighbor The neighbor
 * @param packet The packet
 *
 * @return a status code
 */
retcode_t neighbor_send_packet(node_t *const node, neighbor_t *const neighbor, iota_packet_t const *const packet);

/**
 * Sends transaction flex trits to a neighbor
 *
 * @param node A node
 * @param tangle A tangle
 * @param neighbor The neighbor
 * @param transaction The transaction flex trits
 *
 * @return a status code
 */
retcode_t neighbor_send(node_t *const node, tangle_t *const tangle, neighbor_t *const neighbor,
                        flex_trit_t const *const transaction);

/**
 * Adds a neighbor to a neighbors list
 * The caller must hold the neighbors lock in write access
 *
 * @param neighbors The neighbors list
 * @param neighbor The neighbor
 *
 * @return a status code
 */
retcode_t neighbors_add(neighbor_t **const neighbors, neighbor_t const *const neighbor);

/**
 * Removes a neighbor from a neighbors list
 * The caller must hold the neighbors lock in write access
 *
 * @param neighbors The neighbors list
 * @pram neighbor The neighbor
 *
 * @return a status code
 */
retcode_t neighbors_remove(neighbor_t **const neighbors, neighbor_t *const neighbor);

/**
 * Frees a neighbors list
 * The caller must hold the neighbors lock in write access
 *
 * @param neighbors The neighbors list
 *
 * @return a status code
 */
retcode_t neighbors_free(neighbor_t **const neighbors);

/**
 * Gives the size of the neighbors list
 * The caller must hold the neighbors lock in read access
 *
 * @param neighbors The neighbors list
 *
 * @return the size of the neighbors list
 */
size_t neighbors_count(neighbor_t *const neighbors);

/**
 * Finds a neigbor matching given endpoint
 * The caller must hold the neighbors lock in read access
 *
 * @param neighbors The neighbors list
 * @param endpoint The endpoint
 *
 * @return a pointer to the neigbor if found, NULL otherwise
 */
neighbor_t *neighbors_find_by_endpoint(neighbor_t *const neighbors, endpoint_t const *const endpoint);

/**
 * Finds a neigbor matching given endpoint values
 * The caller must hold the neighbors lock in read access
 *
 * @param neighbors The neighbors list
 * @param ip The endpoint ip
 * @param port The endpoint port
 * @param protocol The endpoint protocol
 *
 * @return a pointer to the neigbor if found, NULL otherwise
 */
neighbor_t *neighbors_find_by_endpoint_values(neighbor_t *const neighbors, char const *const ip, uint16_t const port,
                                              protocol_type_t const protocol);

#ifdef __cplusplus
}
#endif

#endif  // __NODE_NEIGHBOR_H__
