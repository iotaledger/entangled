/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "ciri/node.h"
#include "common/model/transaction.h"
#include "common/network/components/requester.h"
#include "common/network/neighbor.h"
#include "common/network/services/tcp_sender.hpp"
#include "common/network/services/udp_sender.hpp"
#include "common/network/uri_parser.h"
#include "common/trinary/trit_array.h"

bool neighbor_init_with_uri(neighbor_t *const neighbor, char const *const uri) {
  char scheme[MAX_SCHEME_LENGTH];

  if (neighbor == NULL || uri == NULL) {
    return false;
  }
  memset(neighbor, 0, sizeof(neighbor_t));
  if (uri_parse(uri, scheme, MAX_SCHEME_LENGTH, neighbor->endpoint.host,
                MAX_HOST_LENGTH, &neighbor->endpoint.port) == false) {
    return false;
  }
  if (strcmp(scheme, "tcp") == 0) {
    neighbor->endpoint.protocol = PROTOCOL_TCP;
  } else if (strcmp(scheme, "udp") == 0) {
    neighbor->endpoint.protocol = PROTOCOL_UDP;
  } else {
    return false;
  }
  return true;
}

bool neighbor_init_with_values(neighbor_t *const neighbor,
                               protocol_type_t const protocol,
                               char const *const host, uint16_t const port) {
  if (neighbor == NULL || host == NULL) {
    return false;
  }
  memset(neighbor, 0, sizeof(neighbor_t));
  neighbor->endpoint.protocol = protocol;
  if (strlen(host) > MAX_HOST_LENGTH) {
    return false;
  }
  strcpy(neighbor->endpoint.host, host);
  neighbor->endpoint.port = port;
  return true;
}

bool neighbor_send(node_t *const node, neighbor_t *const neighbor,
                   iota_packet_t *const packet) {
  trit_t hash_trits[NUM_TRITS_HASH];
  trit_array_p hash = NULL;

  if (neighbor == NULL) {
    return false;
  }
  if (packet == NULL) {
    return false;
  }
  if (get_transaction_to_request(&node->requester, &hash) == false) {
    return false;
  }
  if (hash != NULL) {
    flex_trit_array_to_int8(hash_trits, NUM_TRITS_HASH, hash->trits,
                            hash->num_trits, hash->num_trits);
    trits_to_bytes(hash_trits, packet->content + PACKET_SIZE, NUM_TRITS_HASH);
  }
  if ((neighbor->endpoint.protocol == PROTOCOL_TCP &&
       tcp_send(&neighbor->endpoint, packet)) ||
      (neighbor->endpoint.protocol == PROTOCOL_UDP &&
       udp_send(&neighbor->endpoint, packet))) {
    neighbor->nbr_sent_tx++;
    return true;
  }
  return false;
}
