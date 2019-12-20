/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_NODE_NETWORK_URI_H__
#define __CIRI_NODE_NETWORK_URI_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define URIS_FOREACH(uris, uri) \
  for (char **uri = (char **)utarray_front(uris); uri != NULL; uri = (char **)utarray_next(uris, uri))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Parses an URI
 *
 * @param uri The URI
 * @param scheme Scheme to be filled
 * @param scheme_len Length of the scheme to be filled
 * @param host Host to be filled
 * @param host_len Length of the host to be filled
 * @param port Port to be filled
 *
 * @returns true if parsing succeeded, false otherwise
 */
bool uri_parse(char const *const uri, char *const scheme, size_t scheme_len, char *const host, size_t host_len,
               uint16_t *const port);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_NODE_NETWORK_URI_H__
