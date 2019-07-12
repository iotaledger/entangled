/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/node/network/neighbor.h"
#include "ciri/node/node.h"
#include "ciri/node/protocol/gossip.h"
#include "ciri/node/protocol/type.h"
#include "common/network/uri.h"
#include "utils/handles/rand.h"

retcode_t neighbor_init_with_uri(neighbor_t *const neighbor, char const *const uri) {
  char scheme[MAX_SCHEME_LENGTH];

  if (neighbor == NULL || uri == NULL) {
    return RC_NULL_PARAM;
  }

  memset(neighbor, 0, sizeof(neighbor_t));
  neighbor->state = NEIGHBOR_DISCONNECTED;
  neighbor->endpoint.stream = NULL;

  if (uri_parse(uri, scheme, MAX_SCHEME_LENGTH, neighbor->endpoint.domain, MAX_HOST_LENGTH, &neighbor->endpoint.port) ==
      false) {
    return RC_NEIGHBOR_FAILED_URI_PARSING;
  }
  if (strcmp(scheme, "tcp") != 0) {
    return RC_NEIGHBOR_INVALID_PROTOCOL;
  }

  return RC_OK;
}

retcode_t neighbor_init_with_values(neighbor_t *const neighbor, char const *const ip, uint16_t const port) {
  if (neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  memset(neighbor, 0, sizeof(neighbor_t));
  neighbor->state = NEIGHBOR_DISCONNECTED;
  neighbor->endpoint.stream = NULL;

  if (ip) {
    if (strlen(ip) > MAX_HOST_LENGTH) {
      return RC_NEIGHBOR_INVALID_HOST;
    }
    strcpy(neighbor->endpoint.ip, ip);
  }
  neighbor->endpoint.port = port;

  return RC_OK;
}

retcode_t neighbor_send_packet(node_t *const node, neighbor_t *const neighbor, protocol_gossip_t const *const packet) {
  if (node == NULL || neighbor == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  neighbor->nbr_sent_txs++;

  return RC_OK;
}

static retcode_t neighbor_send(node_t *const node, tangle_t *const tangle, neighbor_t *const neighbor,
                               protocol_gossip_t *const packet) {
  retcode_t ret = RC_OK;
  flex_trit_t request[FLEX_TRIT_SIZE_243];
  bool is_milestone = rand_handle_probability() < node->conf.p_select_milestone;

  if ((ret = get_transaction_to_request(&node->transaction_requester, tangle, request, is_milestone)) != RC_OK) {
    return ret;
  }

  if ((ret = protocol_gossip_set_request(packet, request, HASH_LENGTH_TRIT - node->conf.mwm)) != RC_OK) {
    return ret;
  }

  return neighbor_send_packet(node, neighbor, packet);
}

retcode_t neighbor_send_trits(node_t *const node, tangle_t *const tangle, neighbor_t *const neighbor,
                              flex_trit_t const *const trits) {
  retcode_t ret = RC_OK;
  protocol_gossip_t packet;

  if (node == NULL || tangle == NULL || neighbor == NULL || trits == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = protocol_gossip_set_transaction(&packet, trits)) != RC_OK) {
    return ret;
  }

  return neighbor_send(node, tangle, neighbor, &packet);
}

retcode_t neighbor_send_bytes(node_t *const node, tangle_t *const tangle, neighbor_t *const neighbor,
                              byte_t const *const bytes) {
  protocol_gossip_t packet;

  if (node == NULL || tangle == NULL || neighbor == NULL || bytes == NULL) {
    return RC_NULL_PARAM;
  }

  memcpy(packet.content, bytes, GOSSIP_MAX_BYTES_LENGTH);

  return neighbor_send(node, tangle, neighbor, &packet);
}
