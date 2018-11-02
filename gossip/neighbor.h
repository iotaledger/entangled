/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_NEIGHBOR_H__
#define __GOSSIP_NEIGHBOR_H__

#include <stdbool.h>

#include "common/errors.h"
#include "common/network/endpoint.h"
#include "common/trinary/flex_trit.h"

// Forward declarations
typedef struct node_s node_t;
typedef struct iota_packet_s iota_packet_t;

typedef struct neighbor_s {
  endpoint_t endpoint;
  unsigned int nbr_all_tx;
  unsigned int nbr_new_tx;
  unsigned int nbr_invalid_tx;
  unsigned int nbr_sent_tx;
  unsigned int nbr_random_tx_req;
  bool flagged;
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
retcode_t neighbor_init_with_uri(neighbor_t *const neighbor,
                                 char const *const uri);

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
retcode_t neighbor_init_with_values(neighbor_t *const neighbor,
                                    char const *const ip, uint16_t const port,
                                    protocol_type_t const protocol);

/**
 * Sends a packet to a neighbor
 *
 * @param node A node
 * @param neighbor The neighbor
 * @param transaction Transaction flex trits
 *
 * @return a status code
 */
retcode_t neighbor_send(node_t *const node, neighbor_t *const neighbor,
                        flex_trit_t const *const transaction);

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_NEIGHBOR_H__
