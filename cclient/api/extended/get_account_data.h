/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_ACCOUNT_DATA_H
#define CCLIENT_API_GET_ACCOUNT_DATA_H

#include "cclient/api/extended/types.h"
#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns an `account_data_t` object, containing account information about
 * `addresses`, `transactions` and the total balance.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {flex_trit_t} seed
 * @param {size_t} security - Security
 * @param {account_data_t} out_account - acount data object
 *
 * @returns {retcode_t}
 * - `INVALID_SEED`
 * - `INVALID_START_OPTION`
 * - `INVALID_START_END_OPTIONS`: Invalid combination of start & end options`
 *
 * Refer:
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetAccountData.ts#L84
 */
retcode_t iota_client_get_account_data(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                       size_t const security, account_data_t* out_account);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_ACCOUNT_DATA_H