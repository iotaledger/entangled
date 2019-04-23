/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_core
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_API_CORE_INIT_H
#define CCLIENT_API_CORE_INIT_H

#include "cclient/http/http.h"
#include "cclient/service.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function should be called before using Core APIs.
 *
 *
 * @param[in] serv client service
 * @return #retcode_t
 */
retcode_t iota_client_core_init(iota_client_service_t* const serv);

/**
 * @brief This function should be called for cleanup.
 *
 * @param[in] serv client service
 */
void iota_client_core_destroy(iota_client_service_t* const serv);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_CORE_INIT_H

/** @} */