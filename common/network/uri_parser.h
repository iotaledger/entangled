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
#include <stdlib.h>

#define PROTOCOL_TCP "tcp"
#define PROTOCOL_UDP "udp"
#define PROTOCOL_HTTP "http"
#define PROTOCOL_HTTPS "https"
#define HOST_IPv4 "8.8.8.8"
#define HOST_IPv6 "FEDC:BA98:7654:3210:FEDC:BA98:7654:3210"
#define HOST_HTTP "node04.iotatoken.nl"
#define PORT "14265"

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
bool uri_parse(char const *const uri, char *const scheme, size_t scheme_len,
               char *const host, size_t host_len, uint16_t *const port);

#ifdef __cplusplus
}
#endif

#endif // __COMMON_NETWORK_URI_PARSER_H__
