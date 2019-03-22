/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_LATEST_INCLUSION_H
#define CCLIENT_API_GET_LATEST_INCLUSION_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fetches inclusion states of given transactions and a list of tips,
 * by calling iota_client_get_inclusion_status` on
 * `latest_solid_subtangle_milestione`.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {hash243_queue_t} transactions - List of transactions hashes
 * @param {get_inclusion_states_res_t} out_states - List of inclusion states
 *
 * @returns {retcode_t}
 * - `INVALID_HASH`: Invalid transaction hash
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetLatestInclusion.ts#L34
 */
retcode_t iota_client_get_latest_inclusion(iota_client_service_t const* const serv, hash243_queue_t const transactions,
                                           get_inclusion_states_res_t* out_states);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_LATEST_INCLUSION_H
