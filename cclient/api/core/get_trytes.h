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
#ifndef CCLIENT_API_GET_TRYTES_H
#define CCLIENT_API_GET_TRYTES_H

#include "cclient/http/http.h"
#include "cclient/request/get_trytes.h"
#include "cclient/response/get_trytes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fetches transaction trytes by a given list of transaction hashes.
 *
 * The transaction trytes can be converted into transaction object(#iota_transaction_t) by calling
 * #transaction_deserialize_from_trits or #transaction_deserialize.
 *
 * @param[in] service client service
 * @param[in] req Transaction hashes
 * @param[out] res Transaction trytes
 * @return #retcode_t
 */
retcode_t iota_client_get_trytes(iota_client_service_t const* const service, get_trytes_req_t const* const req,
                                 get_trytes_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_TRYTES_H

/** @} */