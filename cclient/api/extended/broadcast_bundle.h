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
#ifndef CCLIENT_API_BROADCAST_BUNDLE_H
#define CCLIENT_API_BROADCAST_BUNDLE_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Re-broadcasts all transactions in a bundle.
 *
 * Re-broadcasts all transactions in a bundle given the tail transaction hash. It might be useful when transactions did
 * not properly propagate, particularly in the case of large bundles.
 *
 * @param[in] serv client service
 * @param[in] tail_hash A tail transaction hash
 * @param[out] bundle A transaction bundle
 * @return #retcode_t
 */
retcode_t iota_client_broadcast_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                       bundle_transactions_t* const bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_BROADCAST_BUNDLE_H

/** @} */