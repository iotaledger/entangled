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
 * @param[in] serv Client service.
 * @param[in] seed A seed.
 * @param[in] security The security level of addresses, value could be 1,2,3.
 * @param[in] depth The depth for getting transactions to approve
 * @param[in] mwm The minimum weight magnitude for doing Proof-of-Work
 * @param[in] local_pow If <b>true</b> do local Proof-of-Work, otherwise do remote.
 * @param[in] transfers A list of transfer objects.
 * @param[in] remainder_address Optional, a remainder address.
 * @param[in] reference Optional, hash of transaction to start Random-Walk from
 * @param[in] inputs inputs Optional, A list of input objects.
 * @param[in, out] out_tx_objs A list of bundle transaction objects.
 * @return #retcode_t
 */
retcode_t iota_client_send_transfer(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                    uint8_t security, uint32_t const depth, uint8_t const mwm, bool local_pow,
                                    transfer_array_t const* const transfers, flex_trit_t const* const remainder_address,
                                    flex_trit_t const* const reference, inputs_t const* const inputs,
                                    bundle_transactions_t* out_tx_objs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_SEND_TRANSFER_H

/** @} */