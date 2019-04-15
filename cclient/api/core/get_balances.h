/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_core
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_API_GET_BALANCES_H
#define CCLIENT_API_GET_BALANCES_H

#include "cclient/http/http.h"
#include "cclient/request/get_balances.h"
#include "cclient/response/get_balances.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fetches <b>confirmed<b/> balances of given addresses at the latest solid milestone.
 *
 * If the <b>tips</b> was not provided, the returned balance is correct as of the latest confirmed milestone.
 *
 * @param[in] service client service
 * @param[in] req Address, tips and a threshold for balance calculation.
 * @param[out] res Returned balances list and the referencing tips (or milestone).
 * @return #retcode_t
 */
retcode_t iota_client_get_balances(iota_client_service_t const* const service, get_balances_req_t const* const req,
                                   get_balances_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_BALANCES_H

/** @} */