/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_INPUTS_H
#define CCLIENT_API_GET_INPUTS_H

#include "cclient/api/extended/types.h"
#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates and returns an `Inputs` object by generating addresses and fetching
 * their latest balance.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {flex_trit_t} seed
 * @param {address_opt_t} addr_opt - address options: Starting key index,
 * Security level, Ending Key index.
 * @param {threshold} [options.threshold] - Minimum amount of balance required
 * @param {inputs_t} out_input - Inputs object containg a list of `Address`
 * objects and `totalBalance` field.
 *
 * @returns {retcode_t}
 * - `INVALID_SEED`
 * - `INVALID_SECURITY_LEVEL`
 * - `INVALID_START_OPTION`
 * - `INVALID_START_END_OPTIONS`
 * - `INVALID_THRESHOLD`
 * - `INSUFFICIENT_BALANCE`
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetInputs.ts#L70
 */
retcode_t iota_client_get_inputs(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                 address_opt_t const addr_opt, uint64_t const threshold, inputs_t* const out_input);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_INPUTS_H