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
#ifndef CCLIENT_API_GET_ACCOUNT_DATA_H
#define CCLIENT_API_GET_ACCOUNT_DATA_H

#include "cclient/api/extended/types.h"
#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns an #account_data_t object.
 *
 * Returns an #account_data_t object, containing account information about <b>addresses</b>, <b>transactions</b>,
 * <b>inputs</b> and total account balance.
 *
 * @param[in] serv client service
 * @param[in] seed seed
 * @param[in] security Security level to be used for getting addresses
 * @param[out] out_account the result containing a unused address, used addresses, transactions, and total balance.
 * @return #retcode_t
 */
retcode_t iota_client_get_account_data(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                       uint8_t security, account_data_t* out_account);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_ACCOUNT_DATA_H

/** @} */