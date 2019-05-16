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
#include "common/model/inputs.h"
#include "common/model/transfer.h"
#include "utils/logger_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Prepares the transaction trytes by generating a bundle.
 *
 * @param[in] serv client service
 * @param[in] seed A seed
 * @param[in] security The security level of addresses, value could be 1,2,3.
 * @param[in] transfers A list of transfer objects.
 * @param[in] remainder Optional, a remainder address.
 * @param[in] inputs Optional, A list of input objects.
 * @param[in] validate_inputs True for validating input balances, otherwise will not validate input balances.
 * @param[in] timestamp Optional, a timestamp in ms.
 * @param[in, out] out_bundle A list of bundle transaction objects.
 * @return #retcode_t
 */
retcode_t iota_client_prepare_transfers(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                        uint8_t security, transfer_array_t const* const transfers,
                                        flex_trit_t const* const remainder, inputs_t const* const inputs,
                                        bool validate_inputs, uint64_t timestamp, bundle_transactions_t* out_bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_PREPARE_TRANSFERS_H

/** @} */