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
#ifndef CCLIENT_API_PROMOTE_TRANSACTION_H
#define CCLIENT_API_PROMOTE_TRANSACTION_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Promotes a transaction by adding zero-value spam transactions on top of it.
 *
 * Attempts to promote a transaction using a provided transfers and, if successful.
 * This will effectively double the chances of the transaction to be picked, and this approved.
 *
 * @param[in] serv client service
 * @param[in] tail_hash A tail hash which is the transaction in a bundle with <b>current_index = 0</b>.
 * @param[in] security security level of the address.
 * @param[in] depth The depth as which Random Walk starts, <b>3</b> is a typicall value used by wallets.
 * @param[in] mwm Mininum number of trailing zeros in transaction hash, <b>14</b> for mainnet and <b>9</b> for testnet.
 * @param[in] spam_transfers A list of transfers to promote with.
 * @param[out] out_bundle List of transactions made with the attached transaction objects.
 * @return #retcode_t
 */
retcode_t iota_client_promote_transaction(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                          uint8_t security, int const depth, int const mwm,
                                          transfer_array_t* spam_transfers, bundle_transactions_t* out_bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_PROMOTE_TRANSACTION_H

/** @} */