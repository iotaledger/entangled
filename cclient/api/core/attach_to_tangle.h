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
#ifndef CCLIENT_API_ATTACH_TO_TANGLE_H
#define CCLIENT_API_ATTACH_TO_TANGLE_H

#include "cclient/http/http.h"
#include "cclient/request/attach_to_tangle.h"
#include "cclient/response/attach_to_tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __CCLIENT_LOCAL_POW__

#if defined(__XTENSA__)
#undef __CCLIENT_LOCAL_POW__
#endif

/**
 * @brief Performs Proof-of-Work required to attach a transation to the Tangle.
 *
 * <p>
 * Prepares the specified transactions (trytes) for attachment to the Tangle by doing Proof of Work. You need to supply
 * <b>branch transaction</b> as well as <b>trunk transaction</b>. These are the tips which you're going to validate and
 * reference with this transaction. These are obtainable by the #iota_client_get_transactions_to_approve API call.
 * </p>
 * <p>
 * The returned value is a different set of tryte values which you can input into #iota_client_broadcast_transactions
 * and #iota_client_store_transactions
 * </p>
 *
 * Returns list of transaction trytes and overwrites the following fields:
 *  - `hash`
 *  - `nonce`
 *  - `attachmentTimestamp`
 *  - `attachmentTimestampLowerBound`
 *  - `attachmentTimestampUpperBound`
 *
 * @param[in] service client service
 * @param[in] req Request containing: trunk, branch, minWeightMagnitude, trytes.
 * @param[out] res Array of transaction trytes with nonce and attachment timestamps
 * @return #retcode_t
 */
retcode_t iota_client_attach_to_tangle(iota_client_service_t const* const service,
                                       attach_to_tangle_req_t const* const req, attach_to_tangle_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_ATTACH_TO_TANGLE_H

/** @} */