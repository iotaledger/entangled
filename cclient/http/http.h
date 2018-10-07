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

retcode_t iota_service_query(const void* const service_opaque,
                             char_buffer_t* obj, char_buffer_t* response);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_HTTP_HTTP_H_
