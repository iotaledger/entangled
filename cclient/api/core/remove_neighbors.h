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
#ifndef CCLIENT_API_REMOVE_NEIGHBORS_H
#define CCLIENT_API_REMOVE_NEIGHBORS_H

#include "cclient/http/http.h"
#include "cclient/request/remove_neighbors.h"
#include "cclient/response/remove_neighbors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Removes a list of neighbors from the connected IRI node.
 *
 * Assumes <b>removeNeighbors<b/> command is available on the node. This method has temporary effect until your IRI node
 * relaunches.
 *
 * The URI format:
 *
 * - udp://IPADDRESS:PORT
 * - tcp://IPADDRESS:PORT
 *
 * @param[in] service client service
 * @param[in] req List of URIs that you want to remove from the node.
 * @param[out] res Number of neighbors that were removed.
 * @return #retcode_t
 */
retcode_t iota_client_remove_neighbors(iota_client_service_t const* const service,
                                       remove_neighbors_req_t const* const req, remove_neighbors_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_REMOVE_NEIGHBORS_H

/** @} */