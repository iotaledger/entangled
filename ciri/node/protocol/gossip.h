/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_PROTOCOL_GOSSIP_H__
#define __CIRI_NODE_PROTOCOL_GOSSIP_H__

#include "utlist.h"

#include "ciri/node/network/endpoint.h"
#include "common/errors.h"
#include "common/trinary/bytes.h"
#include "common/trinary/flex_trit.h"

// The amount of bytes making up the non signature message fragment part of a transaction gossip payload
#define GOSSIP_NON_SIG_BYTES_LENGTH 292
// The max amount of bytes a signature message fragment is made up from
#define GOSSIP_SIG_MAX_BYTES_LENGTH 1312
// The amount of bytes used for the transaction part of a gossip payload
#define GOSSIP_TX_BYTES_LENGTH ((GOSSIP_NON_SIG_BYTES_LENGTH) + (GOSSIP_SIG_MAX_BYTES_LENGTH))
// The amount of bytes used for the requested transaction hash
#define GOSSIP_REQUESTED_TX_HASH_BYTES_LENGTH 49
// The minimum amount of bytes making up the transaction gossip payload
#define GOSSIP_MIN_BYTES_LENGTH ((GOSSIP_NON_SIG_BYTES_LENGTH) + (GOSSIP_REQUESTED_TX_HASH_BYTES_LENGTH))
// The maximum amount of bytes making up the transaction gossip payload
#define GOSSIP_MAX_BYTES_LENGTH ((GOSSIP_TX_BYTES_LENGTH) + (GOSSIP_REQUESTED_TX_HASH_BYTES_LENGTH))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The IOTA gossip protocol exchange packet that contains:
 * - A transaction encoded in bytes
 * - A request hash encoded in bytes
 * - A metadata endpoint containing information related to the packet sender
 */
typedef struct __attribute__((__packed__)) protocol_gossip_s {
  byte_t content[GOSSIP_MAX_BYTES_LENGTH];
  endpoint_t source;
} protocol_gossip_t;

/**
 * A queue of gossip packets used to dispatch packets in different threads.
 * Not concurrent by default.
 */
typedef struct protocol_gossip_queue_entry_s {
  protocol_gossip_t packet;
  struct protocol_gossip_queue_entry_s* next;
  struct protocol_gossip_queue_entry_s* prev;
} protocol_gossip_queue_entry_t;

typedef protocol_gossip_queue_entry_t* protocol_gossip_queue_t;

/**
 * Sets the transaction of a gossip packet
 *
 * @param[out]  packet      The gossip packet
 * @param[in]   transaction The transaction flex trits
 *
 * @return a status code
 */
retcode_t protocol_gossip_set_transaction(protocol_gossip_t* const packet, flex_trit_t const* const transaction);

/**
 * Sets the request of a gossip packet
 *
 * @param[out]  packet        The gossip packet
 * @param[in]   request       The request flex trits
 * @param[in]   request_size  The size of the request hash in trits
 *
 * @return a status code
 */
retcode_t protocol_gossip_set_request(protocol_gossip_t* const packet, flex_trit_t const* const request,
                                      uint8_t request_size);

/**
 * Sets the endpoint of a gossip packet
 *
 * @param[out]  packet  The gossip packet
 * @param[in]   ip      The endpoint ip
 * @param[in]   port    The endpoint port
 *
 * @return a status code
 */
retcode_t protocol_gossip_set_endpoint(protocol_gossip_t* const packet, char const* const ip, uint16_t const port);

/**
 * Tells whether a gossip packet queue is empty or not
 *
 * @param[in] queue The gossip packet queue
 *
 * @return true if empty, false otherwise
 */
bool protocol_gossip_queue_empty(protocol_gossip_queue_t const queue);

/**
 * Gets the size of a gossip packet queue
 *
 * @param[in] queue The gossip packet queue
 *
 * @return the size of the packet queue
 */
size_t protocol_gossip_queue_count(protocol_gossip_queue_t const queue);

/**
 * Pushes a packet to a gossip packet queue
 *
 * @param[out]  queue   The gossip packet queue
 * @param[in]   packet  The gossip packet
 *
 * @return a status code
 */
retcode_t protocol_gossip_queue_push(protocol_gossip_queue_t* const queue, protocol_gossip_t const* const packet);

/**
 * Pops a packet from a gossip packet queue
 *
 * @param[out]  queue The gossip packet queue
 *
 * @return the gossip packet
 */
protocol_gossip_queue_entry_t* protocol_gossip_queue_pop(protocol_gossip_queue_t* const queue);

/**
 * Frees a gossip packet queue
 *
 * @param[out]  queue The gossip packet queue
 *
 * @return a status code
 */
void protocol_gossip_queue_free(protocol_gossip_queue_t* const queue);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_NODE_PROTOCOL_GOSSIP_H__
