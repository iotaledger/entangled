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
#ifndef CCLIENT_API_GET_TRANSACTION_OBJECTS_H
#define CCLIENT_API_GET_TRANSACTION_OBJECTS_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fetches the transaction objects, given a list of transaction hashes.
 *
 * @param[in] serv client service
 * @param[in] tx_hashes List of transaction hashes
 * @param[out] out_tx_objs List of transaction objects
 * @return #retcode_t
 */
retcode_t iota_client_get_transaction_objects(iota_client_service_t const* const serv,
                                              get_trytes_req_t* const tx_hashes, transaction_array_t* out_tx_objs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_TRANSACTION_OBJECTS_H

/** @} */