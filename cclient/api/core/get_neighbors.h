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
#ifndef CCLIENT_API_GET_NEIGHBORS_H
#define CCLIENT_API_GET_NEIGHBORS_H

#include "cclient/http/http.h"
#include "cclient/response/get_neighbors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns a list of connected neighbors.
 *
 * @param[in] service client service
 * @param[out] res A list of neighbors.
 * @return #retcode_t
 */
retcode_t iota_client_get_neighbors(iota_client_service_t const* const service, get_neighbors_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_NEIGHBORS_H

/** @} */