/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/model/transaction.h"
#include "common/network/uri_parser.h"
#include "common/trinary/trit_array.h"
#include "gossip/iota_packet.h"
#include "gossip/neighbor.h"
#include "gossip/node.h"
#include "gossip/services/tcp_sender.hpp"
#include "gossip/services/udp_sender.hpp"
#include "utils/handles/rand.h"

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
    strcpy(neighbor->endpoint.ip, neighbor->endpoint.host);
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
  if ((ret = get_transaction_to_request(&node->transaction_requester, request,
                                        is_milestone)) != RC_OK) {
    return ret;
  }
  if ((ret = iota_packet_set_request(
           &packet, request, node->conf.request_hash_size_trit)) != RC_OK) {
    return ret;
  }

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
