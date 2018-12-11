/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/model/transaction.h"
#include "gossip/iota_packet.h"

struct lfds711_queue_umm_element queue_element_dummy;

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

bool iota_packet_queue_empty(const iota_packet_queue_t *const queue) {
  return iota_packet_queue_count(queue) == 0;
}

size_t iota_packet_queue_count(const iota_packet_queue_t *const queue) {
  lfds711_pal_uint_t count;
  lfds711_queue_umm_query(
      queue, LFDS711_QUEUE_UMM_QUERY_SINGLETHREADED_GET_COUNT, NULL, &count);
  return count;
}

retcode_t iota_packet_queue_enqueue(iota_packet_queue_t *queue,
                                    iota_packet_t const *const packet) {
  iota_packet_t *p = NULL;

  if (queue == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  if ((p = malloc(sizeof(iota_packet_t))) == NULL) {
    return RC_OOM;
  }

  memcpy(p, packet, sizeof(iota_packet_t));

  LFDS711_QUEUE_UMM_SET_VALUE_IN_ELEMENT(p->qe, p);

  lfds711_queue_umm_enqueue(queue, &p->qe);
  return RC_OK;
}

iota_packet_t *iota_packet_queue_dequeue(iota_packet_queue_t *const queue) {
  struct lfds711_queue_umm_element *qe;
  iota_packet_t *p = NULL;

  if (queue == NULL) {
    return NULL;
  }

  if (!lfds711_queue_umm_dequeue(queue, &qe)) {
    return NULL;
  }

  return LFDS711_QUEUE_UMM_GET_VALUE_FROM_ELEMENT(*qe);
}

static void iota_packet_queue_umm_element_free(
    struct lfds711_queue_umm_state *qumms,
    struct lfds711_queue_umm_element *qumme,
    enum lfds711_misc_flag dummy_element_flag) {
  iota_packet_t *p = LFDS711_QUEUE_UMM_GET_VALUE_FROM_ELEMENT(*qumme);

  free(p);
}

void iota_packet_queue_free(iota_packet_queue_t *queue) {
  if (queue == NULL) {
    return;
  }

  lfds711_queue_umm_cleanup(queue, &iota_packet_queue_umm_element_free);
}

void iota_packet_queue_init_owner(iota_packet_queue_t *queue) {
  lfds711_queue_umm_init_valid_on_current_logical_core(
      queue, &queue_element_dummy, NULL);
}

void iota_packet_queue_init_user(iota_packet_queue_t *queue) {
  LFDS711_MISC_MAKE_VALID_ON_CURRENT_LOGICAL_CORE_INITS_COMPLETED_BEFORE_NOW_ON_ANY_OTHER_LOGICAL_CORE;
}
