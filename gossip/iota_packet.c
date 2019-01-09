/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/model/transaction.h"
#include "gossip/iota_packet.h"

retcode_t iota_packet_set_transaction(iota_packet_t *const packet,
                                      flex_trit_t const *const transaction) {
  if (packet == NULL || transaction == NULL) {
    return RC_NULL_PARAM;
  }

  if (flex_trits_to_bytes(packet->content, NUM_TRITS_SERIALIZED_TRANSACTION,
                          transaction, NUM_TRITS_SERIALIZED_TRANSACTION,
                          NUM_TRITS_SERIALIZED_TRANSACTION) !=
      NUM_TRITS_SERIALIZED_TRANSACTION) {
    return RC_GOSSIP_SET_PACKET_TRANSACTION_FAILED;
  }

  return RC_OK;
}

retcode_t iota_packet_set_request(iota_packet_t *const packet,
                                  flex_trit_t const *const request,
                                  uint8_t request_size) {
  if (packet == NULL || request == NULL) {
    return RC_NULL_PARAM;
  }

  if (flex_trits_to_bytes(packet->content + PACKET_TX_SIZE, request_size,
                          request, HASH_LENGTH_TRIT,
                          request_size) != request_size) {
    return RC_GOSSIP_SET_PACKET_REQUEST_FAILED;
  }

  return RC_OK;
}

retcode_t iota_packet_set_endpoint(iota_packet_t *const packet,
                                   char const *const ip, uint16_t const port,
                                   protocol_type_t const protocol) {
  if (packet == NULL) {
    return RC_NULL_PARAM;
  }

  if (ip != NULL) {
    strcpy(packet->source.ip, ip);
  }
  packet->source.port = port;
  packet->source.protocol = protocol;

  return RC_OK;
}
