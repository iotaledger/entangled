/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_TRANSACTION_OBJECTS_H
#define CCLIENT_API_GET_TRANSACTION_OBJECTS_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fetches the transaction objects, given an array of transaction hashes.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {hashes_t} hashes - Array of transaction hashes
 * @param {transaction_array_t} tx_objs - List of transaction objects.
 *
 * @returns {retcode_t}
 * - `INVALID_TRANSACTION_HASH`
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetTransactionObjects.ts#L35
 */
retcode_t iota_client_get_transaction_objects(iota_client_service_t const* const serv,
                                              get_trytes_req_t* const tx_hashes, transaction_array_t* out_tx_objs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_TRANSACTION_OBJECTS_H
