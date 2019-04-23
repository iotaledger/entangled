/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_core
 *
 * @{
 *
 * @file
 * @brief
 *
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
 * @brief Searches transactions.
 *
 * It allows to search transactions by <b>addresses, tags, and approvees</b>
 *
 * @param[in] service client service
 * @param[in] req The query objects
 * @param[out] res Transaction hashes
 * @return #retcode_t
 */
retcode_t iota_client_find_transactions(iota_client_service_t const* const service,
                                        find_transactions_req_t const* const req, find_transactions_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_FIND_TRANSACTIONS_H

/** @} */