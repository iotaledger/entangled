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

#include "cclient/api/extended/types.h"
#include "cclient/http/http.h"
#include "common/model/inputs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns an list of `input_t` with a total balance field.
 *
 * @param serv client service
 * @param seed a seed for the address generator
 * @param addr_opt the security level and key indices
 * @param threshold Minimum amount of balance required
 * @param out_inputs the result of inputs
 * @return retcode_t error code
 */
retcode_t iota_client_get_inputs(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                 address_opt_t const addr_opt, uint64_t const threshold, inputs_t* const out_inputs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_INPUTS_H

/** @} */