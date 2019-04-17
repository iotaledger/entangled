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
#ifndef CCLIENT_API_SEND_TRANSFER_H
#define CCLIENT_API_SEND_TRANSFER_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"
#include "common/model/transfer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Wrapper function: Runs #iota_client_prepare_transfers and #iota_client_send_trytes
 *
 * @param[in] serv client service
 * @param[in] depth The depth for getting transactions to approve
 * @param[in] mwm The minimum weight magnitude for doing Proof-of-Work
 * @param[in] local_pow If <b>true</b> do local Proof-of-Work, otherwise do remote.
 * @param[in] transfers List of transfer objects
 * @param[in] num_transfer Number of transfer objects
 * @param[in] reference Optional, hash of transaction to start Random-Walk from
 * @param[out] out_tx_objs List of transaction objects
 * @return #retcode_t
 */
retcode_t iota_client_send_transfer(iota_client_service_t const* const serv, int const depth, int const mwm,
                                    bool local_pow, transfer_t** const transfers, uint32_t num_transfer,
                                    flex_trit_t const* const reference, bundle_transactions_t* out_tx_objs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_SEND_TRANSFER_H

/** @} */