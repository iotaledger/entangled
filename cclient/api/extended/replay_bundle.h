/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_REPLAY_BUNDLE_H
#define CCLIENT_API_REPLAY_BUNDLE_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reattaches a transfer to tangle by selecting tips & performing the
 * Proof-of-Work again. Reattachments are usefull in case original transactions
 * are pending, and can be done securely as many times as needed.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {flex_trit_t} tail - Tail transaction hash. Tail transaction is the
 * transaction in the bundle with `currentIndex == 0`.
 * @param {int} depth - The depth at which Random Walk starts. A value of `3`
 * is typically used by wallets, meaning that RW starts 3 milestones back.
 * @param {int} mwm- Minimum number of trailing zeros in
 * transaction hash. This is used by iota_client_attach_to_tangle() function to
 * search for a valid `nonce`. Currently is `14` on mainnet & spamnnet and `9`
 * on most other testnets.
 * @param {bundle_transactions_t} bundle - Analyzed Transaction objects.
 *
 *
 * @returns {retcode_t}
 * - `INVALID_DEPTH`
 * - `INVALID_MIN_WEIGHT_MAGNITUDE`
 * - `INVALID_TRANSACTION_HASH`
 * - `INVALID_BUNDLE`
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createReplayBundle.ts#L38
 */
retcode_t iota_client_replay_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                    int const depth, int const mwm, bundle_transactions_t* const bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_REPLAY_BUNDLE_H