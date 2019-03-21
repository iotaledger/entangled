/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_INCLUSION_STATES_H
#define CCLIENT_API_GET_INCLUSION_STATES_H

#include "cclient/http/http.h"
#include "cclient/request/get_inclusion_states.h"
#include "cclient/response/get_inclusion_states.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the inclusion states of a set of transactions. This is for determining if
 * a transaction was accepted and confirmed by the network or not. You can
 * search for multiple tips (and thus, milestones) to get past inclusion states
 * of transactions. This API call simply returns a list of boolean values in the
 * same order as the transaction list you submitted, thus you get a true/false
 * whether a transaction is confirmed or not.
 *
 * https://iota.readme.io/reference#getinclusionstates
 *
 * @param service IRI node end point.
 * @param req Request containing transactions to which we are searching for
 * inclusion state for.
 * @param res Response containing the corresponding inclusion states
 *
 * @return error value.
 */
retcode_t iota_client_get_inclusion_states(const iota_client_service_t* const service,
                                           get_inclusion_states_req_t* const req, get_inclusion_states_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_INCLUSION_STATES_H
