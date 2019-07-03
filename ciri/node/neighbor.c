/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/node/neighbor.h"
#include "ciri/node/node.h"
#include "ciri/node/protocol/iota_packet.h"
#include "common/network/uri.h"
#include "utils/handles/rand.h"

retcode_t neighbor_init_with_uri(neighbor_t *const neighbor, char const *const uri) {
  char scheme[MAX_SCHEME_LENGTH];

  if (neighbor == NULL || uri == NULL) {
    return RC_NULL_PARAM;
  }

  memset(neighbor, 0, sizeof(neighbor_t));
  if (uri_parse(uri, scheme, MAX_SCHEME_LENGTH, neighbor->endpoint.host, MAX_HOST_LENGTH, &neighbor->endpoint.port) ==
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
  if (ip) {
    if (strlen(ip) > MAX_HOST_LENGTH) {
      return RC_NEIGHBOR_INVALID_HOST;
    }
    strcpy(neighbor->endpoint.ip, ip);
  }
  neighbor->endpoint.port = port;
  return RC_OK;
}

retcode_t neighbor_send_packet(node_t *const node, neighbor_t *const neighbor, iota_packet_t const *const packet) {
  if (node == NULL || neighbor == NULL || packet == NULL) {
    return RC_NULL_PARAM;
  }

  neighbor->nbr_sent_txs++;

  return RC_OK;
}

static retcode_t neighbor_send(node_t *const node, tangle_t *const tangle, neighbor_t *const neighbor,
                               iota_packet_t *const packet) {
  retcode_t ret = RC_OK;
  flex_trit_t request[FLEX_TRIT_SIZE_243];
  bool is_milestone = rand_handle_probability() < node->conf.p_select_milestone;

  if ((ret = get_transaction_to_request(&node->transaction_requester, tangle, request, is_milestone)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_packet_set_request(packet, request, node->conf.request_hash_size_trit)) != RC_OK) {
    return ret;
  }

  return neighbor_send_packet(node, neighbor, packet);
}

retcode_t neighbor_send_trits(node_t *const node, tangle_t *const tangle, neighbor_t *const neighbor,
                              flex_trit_t const *const trits) {
  retcode_t ret = RC_OK;
  iota_packet_t packet;

  if (node == NULL || tangle == NULL || neighbor == NULL || trits == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = iota_packet_set_transaction(&packet, trits)) != RC_OK) {
    return ret;
  }

  return neighbor_send(node, tangle, neighbor, &packet);
}

retcode_t neighbor_send_bytes(node_t *const node, tangle_t *const tangle, neighbor_t *const neighbor,
                              byte_t const *const bytes) {
  iota_packet_t packet;

  if (node == NULL || tangle == NULL || neighbor == NULL || bytes == NULL) {
    return RC_NULL_PARAM;
  }

  memcpy(packet.content, bytes, PACKET_SIZE);

  return neighbor_send(node, tangle, neighbor, &packet);
}

static int neighbor_cmp(neighbor_t const *const lhs, neighbor_t const *const rhs) {
  if (lhs == NULL || rhs == NULL) {
    return false;
  }

  return !endpoint_cmp(&lhs->endpoint, &rhs->endpoint);
}

retcode_t neighbors_add(neighbor_t **const neighbors, neighbor_t const *const neighbor) {
  neighbor_t *entry = NULL;
  neighbor_t *elt = NULL;

  if (neighbors == NULL || neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  LL_SEARCH(*neighbors, elt, neighbor, neighbor_cmp);

  if (elt != NULL) {
    return RC_NEIGHBOR_ALREADY_PAIRED;
  }

  if ((entry = (neighbor_t *)malloc(sizeof(neighbor_t))) == NULL) {
    return RC_OOM;
  }

  memcpy(entry, neighbor, sizeof(neighbor_t));
  LL_PREPEND(*neighbors, entry);

  return RC_OK;
}

retcode_t neighbors_remove_entry(neighbor_t **const neighbors, neighbor_t *const neighbor) {
  retcode_t ret = RC_OK;

  if (neighbors == NULL || neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  LL_DELETE(*neighbors, neighbor);
  free(neighbor);

  return ret;
}

retcode_t neighbors_remove(neighbor_t **const neighbors, neighbor_t *const neighbor) {
  neighbor_t *elt = NULL;

  if (neighbors == NULL || neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  LL_SEARCH(*neighbors, elt, neighbor, neighbor_cmp);

  if (elt == NULL) {
    return RC_NEIGHBOR_NOT_PAIRED;
  }

  return neighbors_remove_entry(neighbors, elt);
}

retcode_t neighbors_free(neighbor_t **const neighbors) {
  neighbor_t *elt = NULL;
  neighbor_t *tmp = NULL;
  retcode_t ret = RC_OK;

  LL_FOREACH_SAFE(*neighbors, elt, tmp) { ret = neighbors_remove_entry(neighbors, elt); }

  return ret;
}

size_t neighbors_count(neighbor_t *const neighbors) {
  size_t count = 0;
  neighbor_t *elt = NULL;

  if (neighbors == NULL) {
    return 0;
  }

  LL_COUNT(neighbors, elt, count);
  return count;
}

neighbor_t *neighbors_find_by_endpoint(neighbor_t *const neighbors, endpoint_t const *const endpoint) {
  if (neighbors == NULL || endpoint == NULL) {
    return NULL;
  }

  return neighbors_find_by_endpoint_values(neighbors, endpoint->ip, endpoint->port);
}

neighbor_t *neighbors_find_by_endpoint_values(neighbor_t *const neighbors, char const *const ip, uint16_t const port) {
  neighbor_t cmp;
  neighbor_t *elt;

  if (neighbors == NULL) {
    return NULL;
  }

  if (neighbor_init_with_values(&cmp, ip, port)) {
    return NULL;
  }

  LL_SEARCH(neighbors, elt, &cmp, neighbor_cmp);

  return elt;
}
