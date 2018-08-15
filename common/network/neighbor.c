/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

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
                                    protocol_type_t const protocol,
                                    char const *const host,
                                    uint16_t const port) {
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

retcode_t neighbor_send(neighbor_t *const neighbor, trit_array_p const hash) {
  if (neighbor == NULL) {
    return RC_NEIGHBOR_NULL_NEIGHBOR;
  }
  if (hash == NULL) {
    return RC_NEIGHBOR_NULL_HASH;
  }
  if (neighbor->endpoint.protocol == PROTOCOL_TCP) {
    if (tcp_send(&neighbor->endpoint, hash) == false) {
      return RC_NEIGHBOR_FAILED_SEND;
    }
  } else if (neighbor->endpoint.protocol == PROTOCOL_UDP) {
    if (udp_send(&neighbor->endpoint, hash) == false) {
      return RC_NEIGHBOR_FAILED_SEND;
    }
  }
  return RC_OK;
}
