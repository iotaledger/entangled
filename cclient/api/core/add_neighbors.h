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
 * @brief
 *
 * Add a list of neighbors to iri node. It should be noted that this is only
 * temporary, and the added neighbors will be removed from your set of neighbors
 * after you relaunch IRI.
 *
 * @param service client service
 * @param req Request containing list of neighbors to add
 * @param res Response containing a number of neighbor
 * @return
 *  - `#RC_OK`
 *  - `#RC_CCLIENT_OOM`
 *  - `#RC_ERROR`
 */
retcode_t iota_client_add_neighbors(const iota_client_service_t* const service, const add_neighbors_req_t* const req,
                                    add_neighbors_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_ADD_NEIGHBORS_H

/** @} */