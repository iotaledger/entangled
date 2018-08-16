/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_IOTA_PACKET_H__
#define __COMMON_NETWORK_IOTA_PACKET_H__

#include "common/model/transaction.h"
#include "common/network/endpoint.h"
#include "common/trinary/bytes.h"

#define PACKET_SIZE 1650
#define TX_BYTES_SIZE 1604
#define TX_TRITS_SIZE 8019
#define REQ_HASH_BYTES_SIZE 46

typedef struct {
  byte_t content[PACKET_SIZE];
  size_t length;
  endpoint_t source;
} iota_packet_t;

#ifdef __cplusplus
extern "C" {
#endif

void iota_packet_build(iota_packet_t* const packet, size_t const length,
                       char const* const host, uint16_t const port,
                       protocol_type_t const protocol);
void iota_packet_set_transaction(iota_packet_t* const packet,
                                 iota_transaction_t const tx);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_NETWORK_IOTA_PACKET_H__
