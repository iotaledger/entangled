/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_SEND_TRYTES_H
#define CCLIENT_API_SEND_TRYTES_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * [Attaches to tanlge]{@link #module_core.attachToTangle}, [stores]{@link
 * #module_core.storeTransactions} and [broadcasts]{@link
 * #module_core.broadcastTransactions} a list of transaction trytes.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {hash8019_array_p} trytes - List of trytes to attach, store &
 * broadcast
 * @param {uint32_t} depth - Depth
 * @param {uint32_t} mwm - Min weight magnitude
 * @param {flex_trit_t} reference - Optional reference hash
 * @param {bool} local_pow - apply local PoW
 * @param {transaction_array_t} out_transactions - Returns list of attached
 * transactions
 *
 * @return {retcode_t}
 * - Fetch error, if connected to network
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createSendTrytes.ts#L38
 */
retcode_t iota_client_send_trytes(iota_client_service_t const* const serv, hash8019_array_p const trytes,
                                  uint32_t const depth, uint32_t const mwm, flex_trit_t const* const reference,
                                  bool const local_pow, transaction_array_t* out_transactions);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_SEND_TRYTES_H