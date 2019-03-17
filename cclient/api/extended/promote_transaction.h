/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_PROMOTE_TRANSACTION_H
#define CCLIENT_API_PROMOTE_TRANSACTION_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Attempts to promote a transaction using a provided bundle and, if successful,
 * returns the promoting Transactions.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} tail_hash
 * @param {int} depth
 * @param {int} minWeightMagnitude
 * @param {hashes_t} transfers
 *
 * @returns {retcode_t}
 * - `INCONSISTENT SUBTANGLE`: In this case promotion has no effect and
 * reatchment is required.
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createPromoteTransaction.ts#L51
 */
retcode_t iota_client_promote_transaction(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                          int const depth, int const mwm, bundle_transactions_t* const bundle,
                                          bundle_transactions_t* const out_bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_PROMOTE_TRANSACTION_H