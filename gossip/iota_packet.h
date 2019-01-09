/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_IOTA_PACKET_H__
#define __GOSSIP_IOTA_PACKET_H__

#include "common/errors.h"
#include "common/network/endpoint.h"
#include "common/trinary/bytes.h"
#include "common/trinary/flex_trit.h"
#include "gossip/conf.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * Sets the transaction of a packet
 *
 * @param packet The packet
 * @param transaction The transaction flex trits
 *
 * @return a status code
 */
retcode_t iota_packet_set_transaction(iota_packet_t* const packet,
                                      flex_trit_t const* const transaction);

/**
 * Sets the request of a packet
 *
 * @param packet The packet
 * @param request The request flex trits
 * @param request_size The size of the request hash in trits
 *
 * @return a status code
 */
retcode_t iota_packet_set_request(iota_packet_t* const packet,
                                  flex_trit_t const* const request,
                                  uint8_t request_size);

/**
 * Sets the endpoint of a packet
 *
 * @param packet The packet
 * @param ip The endpoint ip
 * @param port The endpoint port
 * @param protocol The endpoint protocol
 *
 * @return a status code
 */
retcode_t iota_packet_set_endpoint(iota_packet_t* const packet,
                                   char const* const ip, uint16_t const port,
                                   protocol_type_t const protocol);

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_IOTA_PACKET_H__
