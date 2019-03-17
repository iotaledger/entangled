/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_BALANCES_H
#define CCLIENT_API_GET_BALANCES_H

#include "cclient/http/http.h"
#include "cclient/request/get_balances.h"
#include "cclient/response/get_balances.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Similar to iota_client_get_inclusion_states. It returns the confirmed balance
 * which a list of addresses have at the latest confirmed milestone. In addition
 * to the balances, it also returns the milestone as well as the index with
 * which the confirmed balance was determined. The balances is returned as a
 * list in the same order as the addresses were provided as input.
 *
 * https://iota.readme.io/reference#getbalances
 *
 * @param service IRI node end point.
 * @param req Request containing the addresses for which balances are
 * requested
 * @param res Response containing the requested balances
 *
 * @return error value.
 */
retcode_t iota_client_get_balances(iota_client_service_t const* const service, get_balances_req_t const* const req,
                                   get_balances_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_BALANCES_H
