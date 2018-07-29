/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "common/network/uri_parser.h"

bool uri_parse(char const *const uri, char *const scheme, char *const host,
               uint16_t *const port) {
  size_t length = 0;
  size_t offset = 0;
  char *delim = 0;

  if (uri == NULL) {
    return false;
  }
  if ((delim = strchr(uri, ':')) == NULL) {
    return false;
  }
  length = (int)(delim - (uri + offset));
  if (length > MAX_SCHEME_LENGTH) {
    return false;
  }
  if (scheme != NULL) {
    memcpy(scheme, uri + offset, length);
    scheme[length] = '\0';
  }
  offset += length;
  if (strncmp(uri + offset, "://", 3) != 0) {
    return false;
  }
  offset += 3;
  if ((delim = strchr(uri + offset, ':')) == NULL) {
    return false;
  }
  length = (int)(delim - (uri + offset));
  if (length > MAX_HOST_LENGTH) {
    return false;
  }
  if (host != NULL) {
    memcpy(host, uri + offset, length);
    host[length] = '\0';
  }
  offset += length + 1;
  if (port != NULL) {
    *port = atoi(uri + offset);
  }
  return true;
}
