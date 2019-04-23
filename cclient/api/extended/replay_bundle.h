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
#ifndef CCLIENT_API_REPLAY_BUNDLE_H
#define CCLIENT_API_REPLAY_BUNDLE_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reattaches a transfer to the Tangle by selecting tips and performing the Proof-of-Work again.
 *
 * Reattachments are useful in case the original transactions are pending, and can be done securely as many times as
 * needed. This will make a new, but identical transaction which now also can be approved. If any of the replayed
 * transactions gets approved, the others stop getting approved.
 *
 * @param[in] serv client service
 * @param[in] tail_hash Tail transaction hash
 * @param[in] depth The depth for getting transactions to approve
 * @param[in] mwm The Minimum Weight Magnitude for doing Proof-of-Work
 * @param[out] bundle Analyzed transaction objects.
 * @return #retcode_t
 */
retcode_t iota_client_replay_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                    int const depth, int const mwm, bundle_transactions_t* const bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_REPLAY_BUNDLE_H

/** @} */