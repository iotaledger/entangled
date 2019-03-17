/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_CHECK_CONSISTENCY_H
#define CCLIENT_API_CHECK_CONSISTENCY_H

#include "cclient/http/http.h"
#include "cclient/request/check_consistency.h"
#include "cclient/response/check_consistency.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Checks if a transaction is _consistent_ or a set of transactions are
 * _co-consistent_.
 * As long as a transaction is consistent it might be accepted
 * by the network. In case transaction is inconsistent, it will not be accepted,
 * and a reattachment is required by calling iota_client_replay_bundle().
 *
 * https://iota.readme.io/reference#checkconsistency
 *
 * @param service IRI node end point.
 * @param req - Request containing tail transaction hash (hash of
 * transaction with `currentIndex=0`), or array of tail transaction hashes.
 * @param res - Response containing consistency state of given transaction
 * or co-consistency of given transactions.
 *
 * @return The error value.
 */
retcode_t iota_client_check_consistency(const iota_client_service_t* const service, check_consistency_req_t* const req,
                                        check_consistency_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_CHECK_CONSISTENCY_H
