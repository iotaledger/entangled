/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_FIND_TRANSACTIONS_H
#define CCLIENT_API_FIND_TRANSACTIONS_H

#include "cclient/http/http.h"
#include "cclient/request/find_transactions.h"
#include "cclient/response/find_transactions.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Find the transactions which attach the specified input and return. All input
 * values are lists (in req), for which a list of return values (transaction
 * hashes) in the same order is returned for all individual elements. The input
 * fields can either be bundles, addresses, tags or approvees. Using multiple of
 * these input fields returns the intersection of the values.
 *
 * https://iota.readme.io/reference#findtransactions
 *
 * @param service IRI node end point.
 * @param req - Request containing all elements by which transactions should be
 * sought.
 * @param res Response containing found transactions
 *
 * @return error value.
 */
retcode_t iota_client_find_transactions(iota_client_service_t const* const service,
                                        find_transactions_req_t const* const req, find_transactions_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_FIND_TRANSACTIONS_H
