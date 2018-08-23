/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_IOTA_PACKET_H__
#define __GOSSIP_IOTA_PACKET_H__

#include "common/network/endpoint.h"
#include "common/trinary/bytes.h"

#define PACKET_SIZE 1650
#define TX_BYTES_SIZE 1604
#define TX_TRITS_SIZE 8019
#define REQ_HASH_BYTES_SIZE 46

// Forward declarations
typedef struct _iota_transaction* iota_transaction_t;
typedef struct _trit_array* trit_array_p;

typedef struct iota_packet_s {
  byte_t content[PACKET_SIZE];
  size_t length;
  endpoint_t source;
} iota_packet_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fills a packet except the content part
 *
 * @param packet The packet
 * @param length The packet length
 * @param host The endpoint host
 * @param port The endpoint port
 * @param protocol The endpoint protocol
 */
void iota_packet_build(iota_packet_t* const packet, size_t const length,
                       char const* const host, uint16_t const port,
                       protocol_type_t const protocol);

/**
 * Fills the transaction part of a packet content
 *
 * @param packet The packet
 * @param tx The transaction
 */
void iota_packet_set_transaction(iota_packet_t* const packet,
                                 iota_transaction_t const tx);

/**
 * Fills the request part of a packet content
 *
 * @param packet The packet
 * @param hash The requested hash
 */
void iota_packet_set_request(iota_packet_t* const packet, trit_array_p hash);

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_IOTA_PACKET_H__
