/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_extended
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_API_FIND_TRANSACTION_OBJECTS_H
#define CCLIENT_API_FIND_TRANSACTION_OBJECTS_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 *
 * Wrapper function for iota_client_find_transactions() and
 * iota_client_get_trytes(). Searches for transactions given a `query` object
 * with `addresses`, `tags` and `approvees` fields. Multiple query fields are
 * supported and `iota_client_find_transaction_objects()` returns intersection
 * of results.
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createFindTransactionObjects.ts#L38
 *
 * @param serv client service
 * @param tx_queries transactions to find
 * @param out_tx_objs A list of transation objects
 * @return error code
 */
retcode_t iota_client_find_transaction_objects(iota_client_service_t const* const serv,
                                               find_transactions_req_t const* const tx_queries,
                                               transaction_array_t* out_tx_objs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_FIND_TRANSACTION_OBJECTS_H

/** @} */