/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_NEW_ADDRESS_H
#define CCLIENT_API_GET_NEW_ADDRESS_H

#include "cclient/api/extended/types.h"
#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generates and returns a new address by calling
 * iota_client_find_transactions() until the first unused address is detected.
 * This stops working after a snapshot.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} seed - At least 81 trytes long seed
 * @param {address_opt_t} addr_opt - address options: Starting key index,
 * Security level, Ending Key index.
 * @param {hash243_queue_t} out_addresses - New (unused) address or list of
 * addresses up to (and including) first unused address.
 *
 * @returns {retcode_t}
 * - `INVALID_SEED`
 * - `INVALID_SECURITY`
 *,
 *   https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetNewAddress.ts#L131
 */
retcode_t iota_client_get_new_address(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                      address_opt_t const addr_opt, hash243_queue_t* out_addresses);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_NEW_ADDRESS_H