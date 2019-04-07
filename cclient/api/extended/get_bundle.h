/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_BUNDLE_H
#define CCLIENT_API_GET_BUNDLE_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fetches and validates the bundle given a _tail_ transaction hash, by calling
 * iota_client_traverse_bundle() and traversing through `trunk transaction`.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {flex_trit_t} tail_hash- Tail transaction hash
 * @param {bundle_transactions_t} bundle - Bundle as array of transaction
 * @param {bundle_status_t} bundle_status - bundle status
 * objects.
 *
 * @returns {retcode_t}
 * - `INVALID_TRANSACTION_HASH`
 * - `INVALID_TAIL_HASH`: Provided transaction is not tail (`currentIndex !==
 * 0`)
 * - `INVALID_BUNDLE`: Bundle is syntactically invalid
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetBundle.ts#L38
 */
retcode_t iota_client_get_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                 bundle_transactions_t* const bundle, bundle_status_t* const bundle_status);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_BUNDLE_H