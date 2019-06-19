/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/model/transaction.h"
#include "node/iota_packet.h"

retcode_t iota_packet_set_transaction(iota_packet_t *const packet, flex_trit_t const *const transaction) {
  if (packet == NULL || transaction == NULL) {
    return RC_NULL_PARAM;
  }

  if (flex_trits_to_bytes(packet->content, NUM_TRITS_SERIALIZED_TRANSACTION, transaction,
                          NUM_TRITS_SERIALIZED_TRANSACTION,
                          NUM_TRITS_SERIALIZED_TRANSACTION) != NUM_TRITS_SERIALIZED_TRANSACTION) {
    return RC_NODE_SET_PACKET_TRANSACTION_FAILED;
  }

  return RC_OK;
}

retcode_t iota_packet_set_request(iota_packet_t *const packet, flex_trit_t const *const request, uint8_t request_size) {
  if (packet == NULL || request == NULL) {
    return RC_NULL_PARAM;
  }

  if (flex_trits_to_bytes(packet->content + PACKET_TX_SIZE, request_size, request, HASH_LENGTH_TRIT, request_size) !=
      request_size) {
    return RC_NODE_SET_PACKET_REQUEST_FAILED;
  }

  return RC_OK;
}

retcode_t iota_packet_set_endpoint(iota_packet_t *const packet, char const *const ip, uint16_t const port,
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

bool iota_packet_queue_empty(iota_packet_queue_t const queue) { return (queue == NULL); }

size_t iota_packet_queue_count(iota_packet_queue_t const queue) {
  iota_packet_queue_entry_t *iter = NULL;
  size_t count = 0;

  CDL_COUNT(queue, iter, count);

  return count;
}

retcode_t iota_packet_queue_push(iota_packet_queue_t *const queue, iota_packet_t const *const packet) {
  iota_packet_queue_entry_t *entry = NULL;

  if (queue == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  if ((entry = (iota_packet_queue_entry_t *)malloc(sizeof(iota_packet_queue_entry_t))) == NULL) {
    return RC_OOM;
  }

  entry->packet = *packet;
  CDL_APPEND(*queue, entry);

  return RC_OK;
}

iota_packet_queue_entry_t *iota_packet_queue_pop(iota_packet_queue_t *const queue) {
  iota_packet_queue_entry_t *front = NULL;

  if (queue == NULL) {
    return NULL;
  }

  front = *queue;
  if (front != NULL) {
    CDL_DELETE(*queue, front);
  }

  return front;
}

void iota_packet_queue_free(iota_packet_queue_t *const queue) {
  iota_packet_queue_entry_t *iter = NULL, *tmp1 = NULL, *tmp2 = NULL;

  if (queue == NULL) {
    return;
  }

  CDL_FOREACH_SAFE(*queue, iter, tmp1, tmp2) {
    CDL_DELETE(*queue, iter);
    free(iter);
  }
  *queue = NULL;
}
