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
#ifndef CCLIENT_API_GET_INPUTS_H
#define CCLIENT_API_GET_INPUTS_H

#include "cclient/api/extended/get_new_address.h"
#include "cclient/http/http.h"
#include "common/model/inputs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets inputs by generating address and fetching balances.
 *
 * Create an #inputs_t object contaning a list of #input_t and total balance.
 *
 * @param[in] serv client service
 * @param[in] seed A seed for address generator
 * @param[in] addr_opt address information
 * @param[in] threshold Minimum balance required
 * @param[out] out_inputs Inputs object containing a list of #input_t and total balance
 * @return #retcode_t
 */
retcode_t iota_client_get_inputs(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                 address_opt_t const addr_opt, int64_t const threshold, inputs_t* const out_inputs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_INPUTS_H

/** @} */