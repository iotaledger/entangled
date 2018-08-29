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
  ip_version_t uri_case;
  const char *possible_scheme[] = {"tcp", "udp", "http", "https"};
  const int possible_scheme_num = sizeof(possible_scheme)/sizeof(*possible_scheme);
  
  if (uri == NULL) {
    return false;
  }
  if ((delim = strchr(uri, ':')) == NULL) {
    return false;
  }
  length = (size_t)(delim - (uri + offset));
  if (length > MAX_SCHEME_LENGTH || length > scheme_len) {
    return false;
  }
  if (scheme) {
    memcpy(scheme, uri + offset, length);
    scheme[length] = '\0';
  }

  // determine if scheme is acceptable type
  for (int i = 0; i < possible_scheme_num; i++) {
    int len;
    if (strlen(possible_scheme[i]) > strlen(scheme)) {
      len = strlen(scheme);
    }
    else {
      len = strlen(possible_scheme[i]);
    }
    if (strncmp(possible_scheme[i], scheme, len) == 0) {
      break;
    }
    else if (i == (possible_scheme_num - 1)) {
      return false;
    }
  }
  offset += length;
  if (strncmp(uri + offset, "://[", 4) == 0) {
    uri_case = IPV6;
    offset += 4;
    if ((delim = strstr(uri + offset, "]:")) == NULL) {
      return false;
    }
  } else if (strncmp(uri + offset, "://", 3) == 0) {
    uri_case = IPV4;  
    
    char *test_delim = delim + 1;
    char *last_delim = delim;
    int count = 1;
    while ((test_delim = strchr(last_delim + 1, ':'))) {
      if ((test_delim -last_delim) <= 6) {
        count++;
      }
      last_delim = test_delim;
      test_delim = last_delim + 1;
      // There are 7 colon in the square bracket and 1 for separate scheme
      if (count >= 8) {
        // if the IPV6 URI comes with no square bracket, this if statement will return false
        return false;
      }
    }
    
    offset += 3;
    if ((delim = strchr(uri + offset, ':')) == NULL) {
      return false;
    }
  } else {
    return false;
  }
  length = (size_t)(delim - (uri + offset));
  if (length > MAX_HOST_LENGTH || length > host_len) {
    return false;
  }
  if (host != NULL) {
    memcpy(host, uri + offset, length);
    host[length] = '\0';
  }
  if (uri_case == IPV6) {
    offset += length + 2;
  } else if (uri_case == IPV4) {
    offset += length + 1;
  }
  if (port != NULL) {
    *port = atoi(uri + offset);
  }
  return true;
}