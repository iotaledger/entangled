/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_FIND_TRANSACTION_OBJECTS_H
#define CCLIENT_API_FIND_TRANSACTION_OBJECTS_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Wrapper function for iota_client_find_transactions() and
 * iota_client_get_trytes(). Searches for transactions given a `query` object
 * with `addresses`, `tags` and `approvees` fields. Multiple query fields are
 * supported and `iota_client_find_transaction_objects()` returns intersection
 * of results.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {find_transactions_req_t} tx_queries - transactions to find.
 * @param {transaction_array_t} tx_objs - List of transaction objects.
 *
 * @returns {retcode_t}
 * - `INVALID_SEARCH_KEY`
 * - `INVALID_HASH`: Invalid bundle hash
 * - `INVALID_TRANSACTION_HASH`: Invalid approvee transaction hash
 * - `INVALID_ADDRESS`: Invalid address
 * - `INVALID_TAG`: Invalid tag
 *
 * Refer:
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createFindTransactionObjects.ts#L38
 */
retcode_t iota_client_find_transaction_objects(iota_client_service_t const* const serv,
                                               find_transactions_req_t const* const tx_queries,
                                               transaction_array_t* out_tx_objs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_FIND_TRANSACTION_OBJECTS_H