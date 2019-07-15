/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_NETWORK_NEIGHBOR_H__
#define __CIRI_NODE_NETWORK_NEIGHBOR_H__

#include <stdbool.h>

#include "utlist.h"

#include "ciri/node/protocol/type.h"
#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct node_s node_t;
typedef struct tangle_s tangle_t;

typedef enum neighbor_state_e {
  NEIGHBOR_DISCONNECTED,
  NEIGHBOR_HANDSHAKING,
  NEIGHBOR_READY_FOR_MESSAGES,
  NEIGHBOR_MARKED_FOR_DISCONNECT,
} neighbor_state_t;

typedef struct neighbor_s {
  byte_t buffer[PACKET_MAX_BYTES_LENGTH];
  size_t buffer_size;
  endpoint_t endpoint;
  neighbor_state_t state;
  uint8_t protocol_version;
  uint64_t nbr_all_txs;
  uint64_t nbr_invalid_txs;
  uint64_t nbr_stale_txs;
  uint64_t nbr_random_tx_reqs;
  uint64_t nbr_sent_txs;
  uint64_t nbr_new_txs;
  uint64_t nbr_dropped_send;
} neighbor_t;

/**
 * Initializes a neighbor with an URI
 *
 * @param[out]  neighbor  The neighbor
 * @param[in]   uri       The URI (e.g tcp://8.8.8.8:14265)
 *
 * @return a status code
 */
retcode_t neighbor_init_with_uri(neighbor_t *const neighbor, char const *const uri);

/**
 * Initializes a neighbor with endpoint values
 *
 * @param[out]  neighbor  The neighbor
 * @param[in]   ip        The endpoint ip
 * @param[in]   port      The endpoint port
 *
 * @return a status code
 */
retcode_t neighbor_init_with_values(neighbor_t *const neighbor, char const *const ip, uint16_t const port);

/**
 * Sends a packet to a neighbor
 *
 * @param[in,out] node      A node
 * @param[in,out] neighbor  The neighbor
 * @param[in]     packet    The packet
 *
 * @return a status code
 */
retcode_t neighbor_send_packet(node_t *const node, neighbor_t *const neighbor, protocol_gossip_t const *const packet);

/**
 * Sends transaction trits to a neighbor
 *
 * @param[in,out] node      A node
 * @param[in]     tangle    A tangle
 * @param[in,out] neighbor  The neighbor
 * @param[in]     trits     The transaction trits
 *
 * @return a status code
 */
retcode_t neighbor_send_trits(node_t *const node, tangle_t const *const tangle, neighbor_t *const neighbor,
                              flex_trit_t const *const trits);

/**
 * Sends transaction bytes to a neighbor
 *
 * @param[in,out] node A node
 * @param[in]     tangle A tangle
 * @param[in,out] neighbor The neighbor
 * @param[in]     bytes The transaction bytes
 *
 * @return a status code
 */
retcode_t neighbor_send_bytes(node_t *const node, tangle_t const *const tangle, neighbor_t *const neighbor,
                              byte_t const *const bytes);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_NODE_NETWORK_NEIGHBOR_H__
