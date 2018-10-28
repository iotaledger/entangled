/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/node.h"
#include "common/model/transaction.h"
#include "common/network/uri_parser.h"
#include "common/trinary/trit_array.h"
#include "gossip/iota_packet.h"
#include "gossip/neighbor.h"
#include "gossip/services/tcp_sender.hpp"
#include "gossip/services/udp_sender.hpp"

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
    if (udp_endpoint_init(&neighbor->endpoint) == false) {
      return RC_NEIGHBOR_FAILED_ENDPOINT_INIT;
    }
  } else {
    return RC_NEIGHBOR_INVALID_PROTOCOL;
  }
  return RC_OK;
}

retcode_t neighbor_init_with_values(neighbor_t *const neighbor,
                                    char const *const ip, uint16_t const port,
                                    protocol_type_t const protocol) {
  if (neighbor == NULL) {
    return RC_NEIGHBOR_NULL_NEIGHBOR;
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

retcode_t neighbor_send(node_t *const node, neighbor_t *const neighbor,
                        flex_trit_t const *const flex_trits) {
  retcode_t ret = RC_OK;
  trit_t hash_trits[HASH_LENGTH_TRIT];
  iota_packet_t packet;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];

  if (node == NULL || neighbor == NULL || flex_trits == NULL) {
    return RC_NULL_PARAM;
  }
  // TODO randomly select milestone or regular tx
  if ((ret = get_transaction_to_request(node->requester, hash, false))) {
    return ret;
  }

  if ((ret = iota_packet_set_transaction(&packet, flex_trits)) != RC_OK) {
    return ret;
  }

  // TODO(thibault): iota_packet_set_request
  flex_trits_to_trits(hash_trits, HASH_LENGTH_TRIT, hash, HASH_LENGTH_TRIT,
                      HASH_LENGTH_TRIT);
  trits_to_bytes(hash_trits, packet.content + PACKET_TX_SIZE, NUM_TRITS_HASH);

  if (neighbor->endpoint.protocol == PROTOCOL_TCP) {
    if (tcp_send(&node->receiver.tcp_service, &neighbor->endpoint, &packet) ==
        false) {
      return RC_NEIGHBOR_FAILED_SEND;
    }
  } else if (neighbor->endpoint.protocol == PROTOCOL_UDP) {
    if (udp_send(&node->receiver.udp_service, &neighbor->endpoint, &packet) ==
        false) {
      return RC_NEIGHBOR_FAILED_SEND;
    }
  } else {
    return RC_NEIGHBOR_INVALID_PROTOCOL;
  }
  neighbor->nbr_sent_tx++;
  return RC_OK;
}
