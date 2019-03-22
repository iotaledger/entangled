/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_BROADCAST_TRANSACTIONS_H
#define CCLIENT_API_BROADCAST_TRANSACTIONS_H

#include "cclient/http/http.h"
#include "cclient/request/broadcast_transactions.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Broadcast a list of transactions to all neighbors. The input trytes for this
 * call are provided by iota_client_attach_to_tangle.
 *
 * https://iota.readme.io/reference#broadcasttransactions
 *
 * @param service IRI node end point.
 * @param req - Request containing the transactions to broadcast
 *
 * @return error value.
 */
retcode_t iota_client_broadcast_transactions(const iota_client_service_t* const service,
                                             broadcast_transactions_req_t* req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_BROADCAST_TRANSACTIONS_H
