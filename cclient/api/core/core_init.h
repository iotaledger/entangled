/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_CORE_INIT_H
#define CCLIENT_API_CORE_INIT_H

#include "cclient/http/http.h"
#include "cclient/service.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t iota_client_core_init(iota_client_service_t* const serv);
void iota_client_core_destroy(iota_client_service_t* const serv);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_CORE_INIT_H
