/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "ciri/node/iota_packet.h"
#include "ciri/node/neighbor.h"
#include "ciri/node/node.h"
#include "ciri/node/services/tcp_sender.hpp"
#include "ciri/node/services/udp_sender.hpp"
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
  if (strcmp(scheme, "tcp") == 0) {
    neighbor->endpoint.protocol = PROTOCOL_TCP;
  } else if (strcmp(scheme, "udp") == 0) {
    neighbor->endpoint.protocol = PROTOCOL_UDP;
  } else {
    return RC_NEIGHBOR_INVALID_PROTOCOL;
  }
  return RC_OK;
}

retcode_t neighbor_init_with_values(neighbor_t *const neighbor, char const *const ip, uint16_t const port,
                                    protocol_type_t const protocol) {
  if (neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  memset(neighbor, 0, sizeof(neighbor_t));
  neighbor->endpoint.protocol = protocol;
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

  if (neighbor->endpoint.protocol == PROTOCOL_TCP) {
    if (tcp_send(&neighbor->endpoint, packet) == false) {
      return RC_NEIGHBOR_FAILED_SEND;
    }
  } else if (neighbor->endpoint.protocol == PROTOCOL_UDP) {
    if (udp_send(&node->receiver.udp_service, &neighbor->endpoint, packet) == false) {
      return RC_NEIGHBOR_FAILED_SEND;
    }
  } else {
    return RC_NEIGHBOR_INVALID_PROTOCOL;
  }

  neighbor->nbr_sent_txs++;

  return RC_OK;
}

retcode_t neighbor_send(node_t *const node, tangle_t *const tangle, neighbor_t *const neighbor,
                        flex_trit_t const *const transaction) {
  retcode_t ret = RC_OK;
  iota_packet_t packet;
  flex_trit_t request[FLEX_TRIT_SIZE_243];

  if (node == NULL || neighbor == NULL || transaction == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = iota_packet_set_transaction(&packet, transaction)) != RC_OK) {
    return ret;
  }

  bool is_milestone = rand_handle_probability() < node->conf.p_select_milestone;

  if ((ret = get_transaction_to_request(&node->transaction_requester, tangle, request, is_milestone)) != RC_OK) {
    return ret;
  }

  if ((ret = iota_packet_set_request(&packet, request, node->conf.request_hash_size_trit)) != RC_OK) {
    return ret;
  }

  return neighbor_send_packet(node, neighbor, &packet);
}

static int neighbor_cmp(neighbor_t const *const lhs, neighbor_t const *const rhs) {
  if (lhs == NULL || rhs == NULL) {
    return false;
  }

  return !((strcmp(lhs->endpoint.ip, rhs->endpoint.ip) == 0 || strcmp(lhs->endpoint.host, rhs->endpoint.host) == 0) &&
           lhs->endpoint.port == rhs->endpoint.port && lhs->endpoint.protocol == rhs->endpoint.protocol);
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

  if (entry->endpoint.protocol == PROTOCOL_UDP) {
    if (udp_endpoint_init(&entry->endpoint) == false) {
      return RC_NEIGHBOR_FAILED_ENDPOINT_INIT;
    }
  } else if (entry->endpoint.protocol == PROTOCOL_TCP) {
    if (tcp_sender_endpoint_init(&entry->endpoint) != RC_OK) {
      return RC_NEIGHBOR_FAILED_ENDPOINT_INIT;
    }
  } else {
    return RC_NEIGHBOR_INVALID_PROTOCOL;
  }

  return RC_OK;
}

retcode_t neighbors_remove_entry(neighbor_t **const neighbors, neighbor_t *const neighbor) {
  retcode_t ret = RC_OK;

  if (neighbors == NULL || neighbor == NULL) {
    return RC_NULL_PARAM;
  }

  if (neighbor->endpoint.protocol == PROTOCOL_UDP) {
    if (udp_endpoint_destroy(&neighbor->endpoint) == false) {
      ret = RC_NEIGHBOR_FAILED_ENDPOINT_DESTROY;
    }
  } else if (neighbor->endpoint.protocol == PROTOCOL_TCP) {
    if (tcp_sender_endpoint_destroy(&neighbor->endpoint) != RC_OK) {
      return RC_NEIGHBOR_FAILED_ENDPOINT_INIT;
    }
  } else {
    return RC_NEIGHBOR_INVALID_PROTOCOL;
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

  return neighbors_find_by_endpoint_values(neighbors, endpoint->ip, endpoint->port, endpoint->protocol);
}

neighbor_t *neighbors_find_by_endpoint_values(neighbor_t *const neighbors, char const *const ip, uint16_t const port,
                                              protocol_type_t const protocol) {
  neighbor_t cmp;
  neighbor_t *elt;

  if (neighbors == NULL) {
    return NULL;
  }

  if (neighbor_init_with_values(&cmp, ip, port, protocol)) {
    return NULL;
  }

  LL_SEARCH(neighbors, elt, &cmp, neighbor_cmp);

  return elt;
}
