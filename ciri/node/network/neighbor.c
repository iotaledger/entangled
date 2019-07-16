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

retcode_t neighbor_init(neighbor_t *const neighbor) {
  if (neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  memset(neighbor, 0, sizeof(neighbor_t));
  neighbor->writer = NULL;
  neighbor->endpoint.stream = NULL;
  neighbor->state = NEIGHBOR_DISCONNECTED;

  return RC_OK;
}

retcode_t neighbor_init_with_uri(neighbor_t *const neighbor, char const *const uri) {
  retcode_t ret = RC_OK;
  char scheme[MAX_SCHEME_LENGTH];

  if (neighbor == NULL || uri == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = neighbor_init(neighbor)) != RC_OK) {
    return ret;
  }

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
  retcode_t ret = RC_OK;

  if (neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = neighbor_init(neighbor)) != RC_OK) {
    return ret;
  }

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
  retcode_t ret = RC_OK;
  protocol_header_t header;
  size_t content_length = GOSSIP_SIG_MAX_BYTES_LENGTH;
  void *buffer = NULL;
  size_t buffer_size = 0;
  neighbor_write_req_t *req = NULL;
  size_t offset = 0;

  if (node == NULL || neighbor == NULL || neighbor->endpoint.stream == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  for (int i = GOSSIP_SIG_MAX_BYTES_LENGTH - 1; i >= 0 && packet->content[i] == 0; i--) {
    content_length--;
  }

  buffer_size = content_length + GOSSIP_NON_SIG_BYTES_LENGTH + GOSSIP_REQUESTED_TX_HASH_BYTES_LENGTH;

  if ((req = malloc(sizeof(neighbor_write_req_t))) == NULL ||
      (buffer = malloc(HEADER_BYTES_LENGTH + buffer_size)) == NULL) {
    return RC_OOM;
  }

  req->neighbor = neighbor;
  req->buf = uv_buf_init(buffer, HEADER_BYTES_LENGTH + buffer_size);

  header.type = GOSSIP;
  header.length = htons(buffer_size);

  memcpy(req->buf.base + offset, &header, HEADER_BYTES_LENGTH);
  offset += HEADER_BYTES_LENGTH;
  memcpy(req->buf.base + offset, packet->content, content_length);
  offset += content_length;
  memcpy(req->buf.base + offset, packet->content + GOSSIP_SIG_MAX_BYTES_LENGTH,
         GOSSIP_NON_SIG_BYTES_LENGTH + GOSSIP_REQUESTED_TX_HASH_BYTES_LENGTH);

  neighbor->writer->data = req;
  uv_async_send(neighbor->writer);

  return ret;
}

static retcode_t neighbor_send(node_t *const node, tangle_t const *const tangle, neighbor_t *const neighbor,
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

retcode_t neighbor_send_trits(node_t *const node, tangle_t const *const tangle, neighbor_t *const neighbor,
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

retcode_t neighbor_send_bytes(node_t *const node, tangle_t const *const tangle, neighbor_t *const neighbor,
                              byte_t const *const bytes) {
  protocol_gossip_t packet;

  if (node == NULL || tangle == NULL || neighbor == NULL || bytes == NULL) {
    return RC_NULL_PARAM;
  }

  memcpy(packet.content, bytes, GOSSIP_MAX_BYTES_LENGTH);

  return neighbor_send(node, tangle, neighbor, &packet);
}
