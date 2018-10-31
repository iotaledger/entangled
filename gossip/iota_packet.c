/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/iota_packet.h"
#include "common/model/transaction.h"

void iota_packet_build(iota_packet_t* const packet, char const* const ip,
                       uint16_t const port, protocol_type_t const protocol) {
  if (packet == NULL) {
    return;
  }
  if (ip != NULL) {
    strcpy(packet->source.ip, ip);
  }
  packet->source.port = port;
  packet->source.protocol = protocol;
}

retcode_t iota_packet_set_transaction(iota_packet_t* const packet,
                                      flex_trit_t const* const transaction) {
  if (packet == NULL || transaction == NULL) {
    return RC_NULL_PARAM;
  }

  flex_trits_to_bytes(packet->content, NUM_TRITS_SERIALIZED_TRANSACTION,
                      transaction, NUM_TRITS_SERIALIZED_TRANSACTION,
                      NUM_TRITS_SERIALIZED_TRANSACTION);

  return RC_OK;
}

retcode_t iota_packet_set_request(iota_packet_t* const packet,
                                  flex_trit_t const* const request,
                                  uint8_t mwm) {
  if (packet == NULL || request == NULL) {
    return RC_NULL_PARAM;
  }

  flex_trits_to_bytes(packet->content + PACKET_TX_SIZE, HASH_LENGTH_TRIT - mwm,
                      request, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT - mwm);

  return RC_OK;
}
