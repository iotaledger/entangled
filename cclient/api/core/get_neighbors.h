/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_NEIGHBORS_H
#define CCLIENT_API_GET_NEIGHBORS_H

#include "cclient/http/http.h"
#include "cclient/response/get_neighbors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns the set of neighbors service node is connected with, as well as their
 * activity counters regarding participation in the protocol. The activity
 * counters are reset after restarting IRI.
 *
 * https://iota.readme.io/reference#getneighborsactivity
 *
 * @param service IRI node end point.
 * @param res Response containing the neighbors' info
 *
 * @return error value.
 */
retcode_t iota_client_get_neighbors(const iota_client_service_t* const service, get_neighbors_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_NEIGHBORS_H
