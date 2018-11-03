/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_IOTA_PACKET_H__
#define __GOSSIP_IOTA_PACKET_H__

#include "utlist.h"

#include "common/errors.h"
#include "common/model/transaction.h"
#include "common/network/endpoint.h"
#include "common/trinary/bytes.h"
#include "common/trinary/flex_trit.h"
#include "gossip/conf.h"

typedef struct iota_packet_s {
  byte_t content[PACKET_SIZE];
  endpoint_t source;
} iota_packet_t;

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
 * Fills a packet except the content part
 *
 * @param packet The packet
 * @param ip The endpoint ip
 * @param port The endpoint port
 * @param protocol The endpoint protocol
 */
void iota_packet_build(iota_packet_t* const packet, char const* const ip,
                       uint16_t const port, protocol_type_t const protocol);

/**
 * Fills the transaction part of a packet content
 *
 * @param packet The packet
 * @param transaction The transaction flex trits
 */
retcode_t iota_packet_set_transaction(iota_packet_t* const packet,
                                      flex_trit_t const* const transaction);

/**
 * Fills the request part of a packet content
 *
 * @param packet The packet
 * @param request The request flex trits
 * @param mwm The minimum weigth magnitude
 */
retcode_t iota_packet_set_request(iota_packet_t* const packet,
                                  flex_trit_t const* const request,
                                  uint8_t mwm);

bool iota_packet_queue_empty(iota_packet_queue_t const queue);
size_t iota_packet_queue_count(iota_packet_queue_t const* const queue);
retcode_t iota_packet_queue_push(iota_packet_queue_t* const queue,
                                 iota_packet_t const* const packet);
void iota_packet_queue_pop(iota_packet_queue_t* const queue);
iota_packet_t* iota_packet_queue_peek(iota_packet_queue_t const queue);
void iota_packet_queue_free(iota_packet_queue_t* const queue);

#ifdef __cplusplus
}
#endif

#endif  // __GOSSIP_IOTA_PACKET_H__
