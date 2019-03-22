/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
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
 * Removes a list of neighbors from your iri node. This is only temporary, and
 * if you have your neighbors added via the command line, they will be retained
 * after you restart your node.
 *
 * https://iota.readme.io/reference#removeneighbors

 * @param service IRI node end point.
 * @param req Request containing list of neighbors to remove.
 * @param res Response containing a number of neighbor
 *
 * @return error value.
 */
retcode_t iota_client_remove_neighbors(const iota_client_service_t* const service,
                                       const remove_neighbors_req_t* const req, remove_neighbors_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_REMOVE_NEIGHBORS_H
