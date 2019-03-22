/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_NODE_INFO_H
#define CCLIENT_API_GET_NODE_INFO_H

#include "cclient/http/http.h"
#include "cclient/response/get_node_info.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns information about your node.
 *
 * https://iota.readme.io/reference#getnodeinfo
 *
 * @param service IRI node end point.
 * @param res Response containing the node's info
 *
 * @return error value.
 */

retcode_t iota_client_get_node_info(const iota_client_service_t* const service, get_node_info_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_NODE_INFO_H
