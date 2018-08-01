/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/network/logger.h"
#include "common/network/neighbor.h"
#include "common/network/services/tcp_sender.hpp"
#include "common/network/uri_parser.h"

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

bool neighbor_send(neighbor_t *const neighbor, trit_array_p const hash) {
  if (neighbor->endpoint.protocol == PROTOCOL_TCP) {
    tcp_send(neighbor->endpoint.opaque_inetaddr);
  } else if (neighbor->endpoint.protocol == PROTOCOL_UDP) {
  } else {
    log_error("Sending to neighbor failed: unrecognized protocol");
    return false;
  }
  // printf("Broadcasting to neighbor %s://%s:%d\n",
  //        neighbor->endpoint.protocol == PROTOCOL_TCP ? "tcp" : "udp",
  //        neighbor->endpoint.host, neighbor->endpoint.port);
  return true;
}
