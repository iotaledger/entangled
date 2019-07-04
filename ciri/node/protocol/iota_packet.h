/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_IOTA_PACKET_H__
#define __NODE_IOTA_PACKET_H__

#include "utlist.h"

#include "common/errors.h"
#include "common/network/endpoint.h"
#include "common/trinary/bytes.h"
#include "common/trinary/flex_trit.h"

#define PACKET_TX_SIZE 1604
#define REQUEST_HASH_SIZE 49
#define PACKET_SIZE (PACKET_TX_SIZE + REQUEST_HASH_SIZE)

/**
 * The IOTA gossip protocol exchange packet that contains:
 * - A transaction encoded in bytes
 * - A request hash encoded in bytes
 * - A metadata endpoint containing information related to the packet sender
 */
typedef struct iota_packet_s {
  byte_t content[PACKET_SIZE];
  endpoint_t source;
} iota_packet_t;

/**
 * A queue of packet used to dispatch packets in different threads.
 * Not concurrent by default.
 */
typedef struct iota_packet_queue_entry_s {
  iota_packet_t packet;
  struct iota_packet_queue_entry_s* next;
  struct iota_packet_queue_entry_s* prev;
} iota_packet_queue_entry_t;

typedef iota_packet_queue_entry_t* iota_packet_queue_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sets the transaction of a packet
 *
 * @param packet The packet
 * @param transaction The transaction flex trits
 *
 * @return a status code
 */
retcode_t iota_packet_set_transaction(iota_packet_t* const packet, flex_trit_t const* const transaction);

/**
 * Sets the request of a packet
 *
 * @param packet The packet
 * @param request The request flex trits
 * @param request_size The size of the request hash in trits
 *
 * @return a status code
 */
retcode_t iota_packet_set_request(iota_packet_t* const packet, flex_trit_t const* const request, uint8_t request_size);

/**
 * Sets the endpoint of a packet
 *
 * @param packet The packet
 * @param ip The endpoint ip
 * @param port The endpoint port
 *
 * @return a status code
 */
retcode_t iota_packet_set_endpoint(iota_packet_t* const packet, char const* const ip, uint16_t const port);

/**
 * Tells whether a packet queue is empty or not
 *
 * @param queue The packet queue
 *
 * @return true if empty, false otherwise
 */
bool iota_packet_queue_empty(iota_packet_queue_t const queue);

/**
 * Gets the size of a packet queue
 *
 * @param queue The packet queue
 *
 * @return the size of the packet queue
 */
size_t iota_packet_queue_count(iota_packet_queue_t const queue);

/**
 * Pushes a packet to a packet queue
 *
 * @param queue The packet queue
 * @param packet The packet
 *
 * @return a status code
 */
retcode_t iota_packet_queue_push(iota_packet_queue_t* const queue, iota_packet_t const* const packet);

/**
 * Pops a packet from a packet queue
 *
 * @param queue The packet queue
 *
 * @return the packet
 */
iota_packet_queue_entry_t* iota_packet_queue_pop(iota_packet_queue_t* const queue);

/**
 * Frees a packet queue
 *
 * @param queue The packet queue
 *
 * @return a status code
 */
void iota_packet_queue_free(iota_packet_queue_t* const queue);

#ifdef __cplusplus
}
#endif

#endif  // __NODE_IOTA_PACKET_H__
