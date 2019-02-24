/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef _COMMON_ERROR_STRINGS_H_
#define _COMMON_ERROR_STRINGS_H_

#ifdef __cplusplus
extern "C" {
#endif
#define STR_OK "OK"

// Cclient Module
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
#define STR_CCLIENT_TX_DESERIALIZE_FAILED \
  "deserializing transaction object failed"
#define STR_CCLIENT_INSUFFICIENT_BALANCE "Insufficient balance"
#define STR_CCLIENT_POW_FAILED "Local PoW failed"
#define SRT_CCLIENT_INVALID_TRANSFER "Invalid transfers"
#define STR_CCLIENT_INVALID_TAIL_HASH "Invalid tail hash"
#define SRT_CCLIENT_INVALID_BUNDLE "Invalid bundle"

#ifdef __cplusplus
}
#endif

#endif  // _COMMON_ERROR_STRINGS_H_
