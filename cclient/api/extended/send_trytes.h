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
#ifndef CCLIENT_API_SEND_TRYTES_H
#define CCLIENT_API_SEND_TRYTES_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Wrapper function: Runs #iota_client_attach_to_tangle and #iota_client_store_and_broadcast
 *
 * @param[in] serv client service
 * @param[in] trytes List of trytes to attach, store and broadcast
 * @param[in] depth The depth at which Random Walk starts. <b>3</b> is typically used by wallets, meaning that RW starts
 * 3 milestones back.
 * @param[in] mwm Minimum number of trailing zeros in transaction hash. This is used to search for a valid <b>nonce</b>.
 * Currently it is <b>14</b> on mainnet & spamnet and <b>9</b> on most other testnets.
 * @param[in] reference Optional, hash of transaction to start Random-Walk from.
 * @param[in] local_pow If <b>true</b> do local Proof-of-Work, otherwise do remote.
 * @param[out] out_transactions the transaction objects were sent to IRI node.
 * @return #retcode_t
 */
retcode_t iota_client_send_trytes(iota_client_service_t const* const serv, hash8019_array_p const trytes,
                                  uint32_t const depth, uint8_t const mwm, flex_trit_t const* const reference,
                                  bool const local_pow, transaction_array_t* out_transactions);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_SEND_TRYTES_H

/** @} */