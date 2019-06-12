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
#ifndef CCLIENT_API_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_API_GET_TRANSACTIONS_TO_APPROVE_H

#include "cclient/http/http.h"
#include "cclient/request/get_transactions_to_approve.h"
#include "cclient/response/get_transactions_to_approve.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Does the <b>tip selection</b>
 *
 * Tip selection which returns <b>trunk</b> and <b>branch</b> transaction. The input value <b>depth</b>
 * determines how many milestones to go back for finding the transactions to approve. The higher your <b>depth</b>
 * value, the more work you have to do as you are confirming more transactions. If the <b>depth</b> is too large
 * (usually above 15, it depends on the node's configuration) an error will be returned. The <b>reference</b> is an
 * optional hash of a transaction you want to approve. If it can't be found at the specified <b>depth</b> then an
 * error will be returned.
 *
 * @param[in] service client service
 * @param[in] req the <b>depth</b> and a <b>reference(optional)</b>
 * @param[out] res A pair of approved transactions(trunk and branch transaction)
 * @return #retcode_t
 */
retcode_t iota_client_get_transactions_to_approve(iota_client_service_t const* const service,
                                                  get_transactions_to_approve_req_t const* const req,
                                                  get_transactions_to_approve_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_TRANSACTIONS_TO_APPROVE_H

/** @} */