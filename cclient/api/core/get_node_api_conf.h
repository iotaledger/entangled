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
#ifndef CCLIENT_API_GET_NODE_API_CONF_H
#define CCLIENT_API_GET_NODE_API_CONF_H

#include "cclient/http/http.h"
#include "cclient/response/get_node_api_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns configuration of the connected node.
 *
 * @param[in] service client service
 * @param[out] res node configuration
 * @return #retcode_t
 */
retcode_t iota_client_get_node_api_conf(iota_client_service_t const* const service, get_node_api_conf_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_NODE_API_CONF_H

/** @} */