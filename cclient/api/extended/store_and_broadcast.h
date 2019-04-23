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
#ifndef CCLIENT_API_STORE_AND_BROADCAST_H
#define CCLIENT_API_STORE_AND_BROADCAST_H

#include "cclient/api/core/broadcast_transactions.h"
#include "cclient/api/core/store_transactions.h"
#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stores and broadcasts a list of <b>attached transaction trytes</b>.
 *
 * <b>Note:</b> Persist the transaction trytes in local storage <I>before</I> calling this command, to ensure that
 * reattachment is possible, until your bundle has been included. Any transactions stored with this command will
 * eventually be erased, as a result of a snapshot.
 *
 * @param[in] serv client service
 * @param[out] trytes List of transaction trytes.
 * @return #retcode_t
 */
retcode_t iota_client_store_and_broadcast(iota_client_service_t const* const serv,
                                          store_transactions_req_t const* const trytes);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_STORE_AND_BROADCAST_H

/** @} */