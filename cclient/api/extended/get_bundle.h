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
#ifndef CCLIENT_API_GET_BUNDLE_H
#define CCLIENT_API_GET_BUNDLE_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the associated bundle transactions from a tail transaction.
 *
 * Fetches and validates the bundle given a <b>tail</b> transaction hash, by calling #iota_client_traverse_bundle
 *
 * @param[in] serv client service
 * @param[in] tail_hash Tail transaction hash
 * @param[out] bundle A Bundle containing transaction objects
 * @param[out] bundle_status Status of bundle validation
 * @return #retcode_t
 */
retcode_t iota_client_get_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                 bundle_transactions_t* const bundle, bundle_status_t* const bundle_status);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_BUNDLE_H

/** @} */