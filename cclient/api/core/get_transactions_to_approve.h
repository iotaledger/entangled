/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
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
 * Tip selection which returns trunkTransaction and branchTransaction. The input
 * value is depth, which basically determines how many bundles to go back to for
 * finding the transactions to approve. The higher your depth value, the more
 * "babysitting" you do for the network (as you have to confirm more
 * transactions).
 *
 * https://iota.readme.io/reference#gettransactionstoapprove
 *
 * @param service IRI node end point.
 * @param req Request containing number of bundles to go back to determine
 * the transactions for approval.
 * @param res Response containing transactions to approve (branch + trunk)
 *
 * @return error value.
 */
retcode_t iota_client_get_transactions_to_approve(const iota_client_service_t* const service,
                                                  const get_transactions_to_approve_req_t* const req,
                                                  get_transactions_to_approve_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_TRANSACTIONS_TO_APPROVE_H
