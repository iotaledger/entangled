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
#ifndef CCLIENT_API_ADD_NEIGHBORS_H
#define CCLIENT_API_ADD_NEIGHBORS_H

#include "cclient/http/http.h"
#include "cclient/request/add_neighbors.h"
#include "cclient/response/add_neighbors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Add a list of neighbors to the IRI node.
 *
 * Assumes <b>addNeighbors</b> command is availiable on the node. It should be noted that this is only temporary, and
 * the added neighbors will be removed from your set of neighbors after you relaunch IRI.
 *
 * The URI format:
 *
 * - udp://IPADDRESS:PORT
 * - tcp://IPADDRESS:PORT
 *
 * @param[in] service client service
 * @param[in] req A list of URIs.
 * @param[out] res Number of neighbors that were added.
 * @return #retcode_t
 */
retcode_t iota_client_add_neighbors(iota_client_service_t const* const service, add_neighbors_req_t const* const req,
                                    add_neighbors_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_ADD_NEIGHBORS_H

/** @} */