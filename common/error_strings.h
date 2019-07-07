/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup common_error
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef _COMMON_ERROR_STRINGS_H_
#define _COMMON_ERROR_STRINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Common error strings
 * @{
 */
#define STR_OK "OK"
#define STR_ERROR "ERROR"
#define STR_NULL_PARAM "Attempting to access a null pointer"
/** @} */

// Cclient Module
/**
 * @name CClient error strings
 * @{
 */
#define STR_CCLIENT_JSON_CREATE "Create JSON object error!"
#define STR_CCLIENT_JSON_PARSE "JSON parsing failed"
#define STR_CCLIENT_OOM "Out of Memory!"
#define STR_CCLIENT_HTTP "HTTP service error"
#define STR_CCLIENT_HTTP_REQ "HTTP request error"
#define STR_CCLIENT_HTTP_RES "HTTP response error"
#define STR_CCLIENT_RES_ERROR "IRI response:"
#define STR_CCLIENT_JSON_KEY "JSON key not found"
#define STR_CCLIENT_FLEX_TRITS "trits converting failed"
#define STR_CCLIENT_NULL_PTR "NULL pointer"
#define STR_CCLIENT_UNIMPLEMENTED "Function unimplemented"
#define STR_CCLIENT_INVALID_SECURITY "Invalid security level"
#define STR_CCLIENT_TX_DESERIALIZE_FAILED "deserializing transaction object failed"
#define STR_CCLIENT_INSUFFICIENT_BALANCE "Insufficient balance"
#define STR_CCLIENT_POW_FAILED "Local PoW failed"
#define SRT_CCLIENT_INVALID_TRANSFER "Invalid transfers"
#define STR_CCLIENT_INVALID_TAIL_HASH "Invalid tail hash"
#define SRT_CCLIENT_INVALID_BUNDLE "Invalid bundle"
#define SRT_CCLIENT_CHECK_BALANCE "Checking balance failed"
#define SRT_CCLIENT_NOT_PROMOTABLE "Unable to promote this transaction"

#define SRT_COMMON_BUNDLE_SIGN "Signing a bundle failed"
/** @} */

/**
 * @name Utils error strings
 * @{
 */
#define STR_UTILS_SOCKET_TLS_RNG "TLS random number generator"
#define STR_UTILS_SOCKET_TLS_CA "TLS parsing CA PEM error"
#define STR_UTILS_SOCKET_TLS_CLIENT_PEM "TLS parsing client Certificate Authority error"
#define STR_UTILS_SOCKET_TLS_CLIENT_PK "TLS parsing client private key error"
#define STR_UTILS_SOCKET_TLS_CONF "TLS setup error"
#define STR_UTILS_SOCKET_TLS_AUTHMODE "TLS configure auth mode error"
#define STR_UTILS_SOCKET_CLIENT_AUTH "TLS set own certificate chain and private key error"
#define STR_UTILS_SOCKET_TLS_HANDSHAKE "TLS handshake error"
#define STR_UTILS_SOCKET_CONNECT "Socket connect error"
#define STR_UTILS_SOCKET_RECV "Socket receive error"
#define STR_UTILS_SOCKET_SEND "Socket send error"
/** @} */

#ifdef __cplusplus
}
#endif

#endif  // _COMMON_ERROR_STRINGS_H_

/** @} */
