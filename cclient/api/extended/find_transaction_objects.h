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
 * @brief Query transaction object by addresses, tags, and approvees.
 *
 * Wrapper function for #iota_client_find_transactions and #iota_client_get_trytes. Searches for transactions given a
 * query object with <b>addresses</b>, <b>tags</b> and <b>approvees</b> fields. Multiple query fields are supported
 * and it returns intersection of results.
 *
 * @param[in] serv client service
 * @param[in] tx_queries transactions to query
 * @param[out] out_tx_objs A list of transation objects
 * @return #retcode_t
 */
retcode_t iota_client_find_transaction_objects(iota_client_service_t const* const serv,
                                               find_transactions_req_t const* const tx_queries,
                                               transaction_array_t* out_tx_objs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_FIND_TRANSACTION_OBJECTS_H

/** @} */