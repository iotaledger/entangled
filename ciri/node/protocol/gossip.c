/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/node/protocol/gossip.h"
#include "common/model/transaction.h"

retcode_t protocol_gossip_set_transaction(protocol_gossip_t *const packet, flex_trit_t const *const transaction) {
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

retcode_t protocol_gossip_set_request(protocol_gossip_t *const packet, flex_trit_t const *const request,
                                      uint8_t request_size) {
  if (packet == NULL || request == NULL) {
    return RC_NULL_PARAM;
  }

  if (flex_trits_to_bytes(packet->content + GOSSIP_TX_BYTES_LENGTH, request_size, request, HASH_LENGTH_TRIT,
                          request_size) != request_size) {
    return RC_NODE_SET_PACKET_REQUEST_FAILED;
  }

  return RC_OK;
}

retcode_t protocol_gossip_set_endpoint(protocol_gossip_t *const packet, char const *const ip, uint16_t const port) {
  if (packet == NULL) {
    return RC_NULL_PARAM;
  }

  if (ip != NULL) {
    strcpy(packet->source.ip, ip);
  }
  packet->source.port = port;

  return RC_OK;
}

bool protocol_gossip_queue_empty(protocol_gossip_queue_t const queue) { return (queue == NULL); }

size_t protocol_gossip_queue_count(protocol_gossip_queue_t const queue) {
  protocol_gossip_queue_entry_t *iter = NULL;
  size_t count = 0;

  CDL_COUNT(queue, iter, count);

  return count;
}

retcode_t protocol_gossip_queue_push(protocol_gossip_queue_t *const queue, protocol_gossip_t const *const packet) {
  protocol_gossip_queue_entry_t *entry = NULL;

  if (queue == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  if ((entry = (protocol_gossip_queue_entry_t *)malloc(sizeof(protocol_gossip_queue_entry_t))) == NULL) {
    return RC_OOM;
  }

  entry->packet = *packet;
  CDL_APPEND(*queue, entry);

  return RC_OK;
}

protocol_gossip_queue_entry_t *protocol_gossip_queue_pop(protocol_gossip_queue_t *const queue) {
  protocol_gossip_queue_entry_t *front = NULL;

  if (queue == NULL) {
    return NULL;
  }

  front = *queue;
  if (front != NULL) {
    CDL_DELETE(*queue, front);
  }

  return front;
}

void protocol_gossip_queue_free(protocol_gossip_queue_t *const queue) {
  protocol_gossip_queue_entry_t *iter = NULL, *tmp1 = NULL, *tmp2 = NULL;

  if (queue == NULL) {
    return;
  }

  CDL_FOREACH_SAFE(*queue, iter, tmp1, tmp2) {
    CDL_DELETE(*queue, iter);
    free(iter);
  }
  *queue = NULL;
}
