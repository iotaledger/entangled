/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/node.h"
#include "common/trinary/trit_array.h"
#include "common/model/transaction.h"
#include "common/network/iota_packet.h"
#include "common/network/neighbor.h"
#include "common/network/services/tcp_sender.hpp"
#include "common/network/services/udp_sender.hpp"
#include "common/network/uri_parser.h"

retcode_t neighbor_init_with_uri(neighbor_t *const neighbor,
                                 char const *const uri) {
  char scheme[MAX_SCHEME_LENGTH];

  if (neighbor == NULL) {
    return RC_NEIGHBOR_NULL_NEIGHBOR;
  }
  if (uri == NULL) {
    return RC_NEIGHBOR_NULL_URI;
  }
  memset(neighbor, 0, sizeof(neighbor_t));
  if (uri_parse(uri, scheme, MAX_SCHEME_LENGTH, neighbor->endpoint.host,
                MAX_HOST_LENGTH, &neighbor->endpoint.port) == false) {
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

retcode_t neighbor_init_with_values(neighbor_t *const neighbor,
                                    char const *const host, uint16_t const port,
                                    protocol_type_t const protocol) {
  if (neighbor == NULL) {
    return RC_NEIGHBOR_NULL_NEIGHBOR;
  }
  memset(neighbor, 0, sizeof(neighbor_t));
  neighbor->endpoint.protocol = protocol;
  if (host) {
    if (strlen(host) > MAX_HOST_LENGTH) {
      return RC_NEIGHBOR_INVALID_HOST;
    }
    strcpy(neighbor->endpoint.host, host);
  }
  neighbor->endpoint.port = port;
  return RC_OK;
}

retcode_t neighbor_send(node_t *const node, neighbor_t *const neighbor,
                        iota_packet_t *const packet) {
  trit_t hash_trits[NUM_TRITS_HASH];
  trit_array_p hash = NULL;

  if (node == NULL) {
    return RC_NEIGHBOR_NULL_NODE;
  }
  if (neighbor == NULL) {
    return RC_NEIGHBOR_NULL_NEIGHBOR;
  }
  if (packet == NULL) {
    return RC_NEIGHBOR_NULL_PACKET;
  }
  if (get_transaction_to_request(&node->requester, &hash) == false) {
    return RC_NEIGHBOR_FAILED_REQUESTER;
  }
  if (hash != NULL) {
    // TODO(thibault): iota_packet_set_request
    flex_trits_to_trits(hash_trits, NUM_TRITS_HASH, hash->trits,
                        hash->num_trits, hash->num_trits);
    trits_to_bytes(hash_trits, packet->content + PACKET_SIZE, NUM_TRITS_HASH);
  }
  if (neighbor->endpoint.protocol == PROTOCOL_TCP) {
    if (tcp_send(&neighbor->endpoint, packet) == false) {
      return RC_NEIGHBOR_FAILED_SEND;
    }
  } else if (neighbor->endpoint.protocol == PROTOCOL_UDP) {
    if (udp_send(&neighbor->endpoint, packet) == false) {
      return RC_NEIGHBOR_FAILED_SEND;
    }
  } else {
    return RC_NEIGHBOR_INVALID_PROTOCOL;
  }
  neighbor->nbr_sent_tx++;
  return RC_OK;
}
