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

iota_api_result_t iota_service_query(const iota_service_t* const service,
                                     const char* const obj,
                                     iota_response_t* const response);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_HTTP_HTTP_H_
