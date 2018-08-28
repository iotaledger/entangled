/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/network/network.h"
#include "common/network/uri_parser.h"

bool uri_parse(char const *const uri, char *const scheme,
               size_t const scheme_len, char *const host, size_t const host_len,
               uint16_t *const port) {
  size_t length = 0;
  size_t offset = 0;
  char *delim = NULL;
  enum URI_CASE {
    ipv4,
    ipv6
  } uri_case;

  if (uri == NULL) {
    return false;
  }
  if ((delim = strchr(uri, ':')) == NULL) {
    return false;
  }
  length = (size_t)(delim - (uri + offset));
  if (length > MAX_SCHEME_LENGTH || length + 1 > scheme_len) {
    return false;
  }
  if (scheme) {
    memcpy(scheme, uri + offset, length);
    scheme[length] = '\0';
  }
  offset += length;
  if (strncmp(uri + offset, "://[", 4) == 0) {
    uri_case = ipv6;
    offset += 4;
    if ((delim = strstr(uri + offset, "]:")) == NULL) {
      return false;
    }
  } else if (strncmp(uri + offset, "://", 3) == 0) {
    uri_case = ipv4;
    offset += 3;
    if ((delim = strchr(uri + offset, ':')) == NULL) {
      return false;
    }
  } else {
    return false;
  }
  length = (size_t)(delim - (uri + offset));
  if (length > MAX_HOST_LENGTH || length + 1 > host_len) {
    return false;
  }
  if (host != NULL) {
    memcpy(host, uri + offset, length);
    host[length] = '\0';
  }
  if (uri_case == ipv6) {
    offset += length + 2;
  } else if (uri_case == ipv4) {
    offset += length + 1;
  }
  if (port != NULL) {
    *port = atoi(uri + offset);
  }
  return true;
}