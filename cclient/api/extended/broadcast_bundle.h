/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_BROADCAST_BUNDLE_H
#define CCLIENT_API_BROADCAST_BUNDLE_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Re-broadcasts all transactions in a bundle given the tail transaction hash.
 * It might be useful when transactions did not properly propagate,
 * particularly in the case of large bundles.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {flex_trit_t} tail_hash - Tail transaction hash
 * @param {bundle_transactions_t} bundle - a bundle object
 *
 * @returns {retcode_t}
 * - `INVALID_HASH`: Invalid tail transaction hash
 * - `INVALID_BUNDLE`: Invalid bundle
 *
 * Refer:
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createBroadcastBundle.ts#L38
 */
retcode_t iota_client_broadcast_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                       bundle_transactions_t* const bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_BROADCAST_BUNDLE_H
