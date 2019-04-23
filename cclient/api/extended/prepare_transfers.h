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
#ifndef CCLIENT_API_PREPARE_TRANSFERS_H
#define CCLIENT_API_PREPARE_TRANSFERS_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"
#include "common/model/transfer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Prepares the transaction trytes by generating a bundle.
 *
 * @param[in] serv client service
 * @param[in] transfers List of transfer objects
 * @param[in] num_transfer Number of transfer objects
 * @param[out] out_bundle Bundle trytes
 * @return #retcode_t
 */
retcode_t iota_client_prepare_transfers(iota_client_service_t const* const serv, transfer_t** const transfers,
                                        uint32_t const num_transfer, bundle_transactions_t* out_bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_PREPARE_TRANSFERS_H

/** @} */