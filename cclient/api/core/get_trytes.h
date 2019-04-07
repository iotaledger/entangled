/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_TRYTES_H
#define CCLIENT_API_GET_TRYTES_H

#include "cclient/http/http.h"
#include "cclient/request/get_trytes.h"
#include "cclient/response/get_trytes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns the raw transaction data (trytes) of a specific transaction. These
 * trytes can then be easily converted into the actual transaction object. See
 * utility functions for more details.
 *
 * https://iota.readme.io/reference#gettrytes
 *
 * @param service IRI node end point.
 * @param req Request containing hashes of transactions to which trytes are
 * to be retrieved
 * @param res Response containing trytes of specific transactions
 *
 * @return error value.
 */
retcode_t iota_client_get_trytes(const iota_client_service_t* const service, get_trytes_req_t* const req,
                                 get_trytes_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_TRYTES_H
