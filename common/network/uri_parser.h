/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_URI_PARSER_H__
#define __COMMON_NETWORK_URI_PARSER_H__

#include <stdbool.h>
#include <stdint.h>

#define MAX_SCHEME_LENGTH 8
#define MAX_HOST_LENGTH 255

bool uri_parse(char const *const uri, char *const scheme, char *const host,
               uint16_t *const port);

#endif  // __COMMON_NETWORK_URI_PARSER_H__
