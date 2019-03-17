/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_HTTP_HTTP_H_
#define CCLIENT_HTTP_HTTP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "cclient/service.h"

// socket buffer can overwrite through the preprocessor macro.
#ifndef RECEIVE_BUFFER_SIZE
#ifdef __XTENSA__
#define RECEIVE_BUFFER_SIZE 2 * 1024
#else
#define RECEIVE_BUFFER_SIZE 4 * 1024
#endif
#endif

extern const char* khttp_ApplicationJson;
extern const char* khttp_ApplicationFormUrlencoded;

retcode_t iota_service_query(const void* const service_opaque, char_buffer_t* obj, char_buffer_t* response);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_HTTP_HTTP_H_
