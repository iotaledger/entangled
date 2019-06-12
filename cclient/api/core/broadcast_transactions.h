/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_core
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_API_BROADCAST_TRANSACTIONS_H
#define CCLIENT_API_BROADCAST_TRANSACTIONS_H

#include "cclient/http/http.h"
#include "cclient/request/broadcast_transactions.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Broadcast a list of transactions to all node's neighbors.
 *
 * These trytes are returned by #iota_client_attach_to_tangle, or by doing proof of work somewhere else.
 *
 * @param[in] service client service
 * @param[in] req The list of transaction trytes to broadcast
 * @return #retcode_t
 */
retcode_t iota_client_broadcast_transactions(iota_client_service_t const* const service,
                                             broadcast_transactions_req_t* req);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_BROADCAST_TRANSACTIONS_H

/** @} */