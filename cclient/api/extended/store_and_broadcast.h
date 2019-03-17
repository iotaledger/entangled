/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_STORE_AND_BROADCAST_H
#define CCLIENT_API_STORE_AND_BROADCAST_H

#include "cclient/api/core/broadcast_transactions.h"
#include "cclient/api/core/store_transactions.h"
#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Stores and broadcasts a list of _attached_ transaction trytes by calling
 * iota_client_store_transactions() and iota_client_broadcast_transactions().
 *
 * Note: Persist the transaction trytes in local storage **before** calling this
 * command, to ensure that reattachment is possible, until your bundle has been
 * included.
 *
 * Any transactions stored with this command will eventaully be erased, as a
 * result of a snapshot.
 *
 * @param {store_transactions_req_t} trytes - Attached transaction trytes
 * @param {hashes_t} out_trytes - Attached transaction trytes
 *
 * @return {retcode_t}
 * @fulfil {Trytes[]} Attached transaction trytes
 * - `INVALID_ATTACHED_TRYTES`: Invalid attached trytes
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createStoreAndBroadcast.ts#L28
 */
retcode_t iota_client_store_and_broadcast(iota_client_service_t const* const serv,
                                          store_transactions_req_t const* const trytes);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_STORE_AND_BROADCAST_H